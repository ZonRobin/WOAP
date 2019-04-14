#include "woap_state_manager.h"
#include "woap_main.h"
#include "woap_user_permission.h"
#include "woap_api_messages_state_manager.h"
#include "woap.h"
#include "woap_api_osc_remote_client.h"

using namespace WOAP;
using namespace WOAP::API;
using namespace WOAP::API::Messages;

SINGLETON_IMPLEMENTATION(StateManager)

StateManager::StateManager()
{
	initializeAPIConsumer();
}

StateManager::~StateManager()
{
	terminateAPIConsumer();
	stopAutoStore();
}

const Bootstrapper::Initializer StateManager::bootstrapInitializer([](Bootstrapper& bootstrapper) {
	bootstrapper.reportPermissions({
		new UserPermission(Permission::HandleStateInternal, "Handle state internaly"),
		new UserPermission(Permission::HandleStateExternal, "Handle state externaly")
	});
});

void StateManager::startAutoStore(int storeIntervalMs)
{
	if (storeIntervalMs <= 0)
	{
		storeWorker = nullptr;
		return;
	}
		
	if (storeWorker == nullptr)
	{
		storeWorker = new StoreWorker(Main::getInstanceCast()->getApplicationStateFile());
		storeWorker->startThread();
	}
		
	storeWorker->setStoreIntervalMs(storeIntervalMs);
}

void StateManager::stopAutoStore()
{
	storeWorker = nullptr;
}

bool StateManager::loadState(bool showAlertOnError) const
{
	File saveFile = Main::getInstanceCast()->getApplicationStateFile();
	if (!saveFile.existsAsFile())
		return false;

	return loadStateFromFile(saveFile, showAlertOnError);
}

bool StateManager::loadStateFromFile(const File& file, bool showAlertOnError) const
{
	const ScopedPointer<XmlElement> stateXml = XmlDocument::parse(file);
	return setState(stateXml, showAlertOnError);
}

void StateManager::stateLoadingFailed() const
{
	AlertWindow::showMessageBox(
		AlertWindow::AlertIconType::WarningIcon,
		"Loading failed",
		"Previous state could not be loaded. The file is currupt. New session has been started.",
		String(),
		nullptr
	);
}

bool StateManager::storeState() const
{
	return storeState(Main::getInstanceCast()->getApplicationStateFile());
}

bool StateManager::storeState(const File& storeFile) const
{
	ASSERT_ONLY_MESSAGE_THREAD;

	const ScopedPointer<XmlElement> stateXml = getState();
	if (stateXml == nullptr) 
	{
		jassertfalse;
		return false;
	}

	if (!storeFile.hasWriteAccess())
	{
		jassertfalse;
		return false;
	}

	return stateXml->writeToFile(storeFile, "", "UTF-8", 9999);
}

XmlElement* StateManager::getState() const
{
	ValueTree appState = Main::getInstanceCast()->getState();
	return appState.createXml();
}

void StateManager::getStateString(OutputStream& stream) const
{
	const ScopedPointer<XmlElement> stateXml = getState();
	if (stateXml == nullptr) 
	{
		jassertfalse;
		return;
	}
	
	stateXml->writeToStream(stream, "", false, true, "UTF-8", 9999);
}

bool StateManager::setState(const XmlElement* stateXml, bool showAlertOnError) const
{
	if (stateXml == nullptr)
	{
		if (showAlertOnError)
			stateLoadingFailed();

		return false;
	}

	const ValueTree state = ValueTree::fromXml(*stateXml);
	if (state.getNumChildren() == 0 || !state.hasType("WOAP"))
	{
		if (showAlertOnError)
			stateLoadingFailed();

		return false;
	}

	Main::getInstanceCast()->setState(state);
	return true;
}

bool StateManager::setState(const String& stateStr, bool showAlertOnError) const
{
	const ScopedPointer<XmlElement> stateXml = XmlDocument::parse(stateStr);
	return setState(stateXml, showAlertOnError);
}

Array<AddressWithTypeId> StateManager::getMessageTypesToListenTo() const
{
	return Array<AddressWithTypeId>({
		AddressWithTypeId(StateManagerSaveState::getAddress(), MessageType::SaveState),
		AddressWithTypeId(StateManagerLoadState::getAddress(), MessageType::LoadState),
		AddressWithTypeId(StateManagerGetState::getAddress(), MessageType::GetState),
		AddressWithTypeId(StateManagerSetState::getAddress(), MessageType::SetState)
	});
}

void StateManager::messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId)
{
	switch (messageTypeId)
	{
	case MessageType::SaveState:
	{
		StateManagerSaveState msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::HandleStateInternal))
			return;

		const bool storeOk = storeState();
		client.sendOSCMessage(StateManagerProcessResult(msg, storeOk));
		break;
	}
	case MessageType::LoadState:
	{
		StateManagerLoadState msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::HandleStateInternal))
			return;

		const bool loadOk = loadState(false);
		client.sendOSCMessage(StateManagerProcessResult(msg, loadOk));
		break;
	}
	case MessageType::GetState:
	{
		StateManagerGetState msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::HandleStateExternal))
			return;

		MemoryOutputStream os;
		getStateString(os);

		client.sendOSCMessage(StateManagerState(msg, os));
		break;
	}
	case MessageType::SetState:
	{
		StateManagerSetState msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::HandleStateExternal))
			return;

		const String stateString = msg.getStateString();
		const bool setOk = setState(stateString, false);

		client.sendOSCMessage(StateManagerProcessResult(msg, setOk));
		break;
	}
	default:
		handleUnknownMessage("StateManager", client, message, messageTypeId);
		break;
	}
}


StateManager::StoreWorker::StoreWorker(const File& storeFile_)
	: Thread("StateManager store worker"), saveFile(storeFile_)
{
}

StateManager::StoreWorker::~StoreWorker()
{
	signalThreadShouldExit();
	notify();

	const ScopedLock l(fileWriterLock);
	stopThread(5000);
}

void StateManager::StoreWorker::run()
{
	while (!threadShouldExit())
	{
		wait(storeIntervalMs);

		if (threadShouldExit())
			return;

		ValueTree appState;

		{
			const MessageManagerLock mmlock;
			appState = Main::getInstanceCast()->getState();
		}

		if (threadShouldExit())
			return;

		const ScopedPointer<XmlElement> stateXml = appState.createXml();
		if (stateXml == nullptr) 
		{
			jassertfalse;
			continue;
		}

		if (threadShouldExit())
			return;

		const ScopedLock l(fileWriterLock);
		if (!saveFile.hasWriteAccess())
		{
			jassertfalse;
			continue;
		}

		const int64 writingStartTime = Time::getApproximateMillisecondCounter();
		if (!stateXml->writeToFile(saveFile, "", "UTF-8", 9999))
		{
			DBG("State writing failed");
		}

		const int64 writingTime = Time::getApproximateMillisecondCounter() - writingStartTime;
		if (writingTime > 2000)
		{
			storeIntervalMs += (int)writingTime;
			DBG("State writing took long (" + String(writingTime) + " ms)");
		}
	}
}