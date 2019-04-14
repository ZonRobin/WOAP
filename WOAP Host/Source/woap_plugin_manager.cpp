#include "woap_plugin_manager.h"
#include "woap_main.h"
#include "woap_api_osc_remote_client.h"
#include "woap_user_permission.h"
#include "woap_api_messages_plugin_manager.h"
#include "woap_api_listener_bridges_plugin_manager_listener_bridge.h"

using namespace WOAP;
using namespace API::Messages;
using namespace API::ListenerBridges;

SINGLETON_IMPLEMENTATION(PluginManager)

PluginManager::PluginManager()
{
	audioPluginFormatManager.addDefaultFormats();
	loadKnownPluginList();

	Main::getInstanceCast()->getPluginsDirectory().createDirectory();

	initializeAPIConsumer();
}

PluginManager::~PluginManager()
{
	terminateAPIConsumer();

	if (scanWorker != nullptr)
		scanWorker->stopThread(1000);
}

const Bootstrapper::Initializer PluginManager::bootstrapInitializer([](Bootstrapper& bootstrapper) {
	bootstrapper.reportPermissions({
		new UserPermission(Permission::View, "View plugin types"),
		new UserPermission(Permission::Scan, "Scan plugin types")
	});
});

void PluginManager::scanPluginTypes()
{
	scanRequested = true;

	if (scanWorker == nullptr)
		scanWorker = new ScanWorker(this);

	if (!scanWorker->isThreadRunning())
		scanWorker->startThread();
}

void PluginManager::scanPluginTypesIfNeeded()
{
	if (!scanRequested)
		scanPluginTypes();
}

const PluginDescription* PluginManager::getPluginType(int pluginId) const
{
	for (PluginDescription* desc : knownPlugins)
	{
		if (desc->uid == pluginId)
			return desc;
	}

	return nullptr;
}

void PluginManager::saveKnownPluginList() const
{
	File pluginList = Main::getInstanceCast()->getPluginListFile();

	const ScopedLock lock(scanLock);

	ScopedPointer<XmlElement> pluginListXml = knownPlugins.createXml();
	pluginListXml->writeToFile(pluginList, "");
}

void PluginManager::loadKnownPluginList()
{
	File pluginList = Main::getInstanceCast()->getPluginListFile();
	if (!pluginList.existsAsFile())
		return;

	const ScopedLock lock(scanLock);

	XmlDocument pluginListXml(pluginList);
	ScopedPointer<XmlElement> rootElement = pluginListXml.getDocumentElement();
	
	if (rootElement == nullptr)
		return;

	knownPlugins.recreateFromXml(*rootElement);
	scanRequested = true;
}

PluginManager::PluginImportData* PluginManager::loadPluginByPath(const String& pluginPath)
{
	OwnedArray<PluginDescription> importPluginTypes;
	KnownPluginList importPluginList;

	importPluginList.scanAndAddDragAndDroppedFiles(audioPluginFormatManager, pluginPath, importPluginTypes);

	if (importPluginTypes.size() == 0)
		return PluginManager::PluginImportData::Fail(PluginImportData::ReturnCode::NoPluginFound);

	if (importPluginTypes.size() > 1)
		return PluginManager::PluginImportData::Fail(PluginImportData::ReturnCode::MoreThanSinglePlugin);

	PluginDescription* pluginDescription = importPluginTypes.removeAndReturn(0);
	
	String error;
	AudioPluginInstance* pluginInstance = audioPluginFormatManager.createPluginInstance(*pluginDescription, 44100, 64, error);
	if (pluginInstance == nullptr)
		return PluginManager::PluginImportData::Fail(PluginImportData::ReturnCode::UnknownError);
	
	return PluginManager::PluginImportData::Success(pluginDescription, pluginInstance);
}

bool PluginManager::importPlugin(const PluginDescription& description)
{
	File pluginFile(description.fileOrIdentifier);
	
	File newPluginFile = Main::getInstanceCast()->getPluginsDirectory().getChildFile(pluginFile.getFileName());
	if (!pluginFile.copyFileTo(newPluginFile))
		return false;

	const ScopedLock lock(scanLock);

	PluginDescription newDescription(description);
	newDescription.fileOrIdentifier = newPluginFile.getFullPathName();

	knownPlugins.addType(newDescription);
	listeners.call(&PluginManager::Listener::pluginTypeAdded, newDescription);

	saveKnownPluginList();
	return true;
}

void PluginManager::ScanWorker::run()
{
	MessageManager::callAsync([this]() {
		pluginManager->listeners.call(&PluginManager::Listener::pluginTypesScanStarted);
	});

	const ScopedLock lock(pluginManager->scanLock);

	pluginManager->knownPlugins.clear();

	FileSearchPath dirToScan(Main::getInstanceCast()->getPluginsDirectory().getFullPathName());
	File blacklist = Main::getInstanceCast()->getPluginBlacklistFile();
	String pluginName;
	const int numFormats = pluginManager->audioPluginFormatManager.getNumFormats();

	for (int i = 0; i < numFormats; ++i)
	{
		if (threadShouldExit()) return;

		PluginDirectoryScanner scanner(pluginManager->knownPlugins, *(pluginManager->audioPluginFormatManager.getFormat(i)), dirToScan, true, blacklist);
		while (scanner.scanNextFile(false, pluginName))
		{
			if (threadShouldExit()) return;
		}
	}

	pluginManager->saveKnownPluginList();

	const int newNumPlugins = pluginManager->knownPlugins.getNumTypes();

	MessageManager::callAsync([this, newNumPlugins]() {
		pluginManager->listeners.call(&PluginManager::Listener::pluginTypesScanned, newNumPlugins);
	});
}


Array<API::AddressWithTypeId> PluginManager::getMessageTypesToListenTo() const
{
	return Array<API::AddressWithTypeId>({
		API::AddressWithTypeId(PluginManagerGetPluginTypes::getAddress(), MessageType::GetPluginTypes),
		API::AddressWithTypeId(PluginManagerScanPluginTypes::getAddress(), MessageType::ScanPluginTypes),
		API::AddressWithTypeId(PluginManagerScanPluginTypesIfNeeded::getAddress(), MessageType::ScanPluginTypesIfNeeded),
		API::AddressWithTypeId(PluginManagerAddListener::getAddress(), MessageType::AddListener),
		API::AddressWithTypeId(PluginManagerRemoveListener::getAddress(), MessageType::RemoveListener)
	});
}

void PluginManager::messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId)
{
	switch(messageTypeId)
	{
	case MessageType::GetPluginTypes:
	{
		PluginManagerGetPluginTypes msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::View))
			return;

		const ScopedLock lock(scanLock);
		client.sendOSCMessage(PluginManagerPluginTypes(msg, Array<PluginDescription*>(knownPlugins.begin(), knownPlugins.getNumTypes())));
		break;
	}
	case MessageType::ScanPluginTypes:
	{
		PluginManagerScanPluginTypes msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::Scan))
			return;

		scanPluginTypes();
		break;
	}
	case MessageType::ScanPluginTypesIfNeeded:
	{
		PluginManagerScanPluginTypesIfNeeded msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::Scan))
			return;

		scanPluginTypesIfNeeded();
		break;
	}
	case MessageType::AddListener:
	{
		PluginManagerAddListener msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::View))
			return;

		PluginManagerListenerBridge::addToList<Listener>(listeners, new PluginManagerListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveListener:
	{
		PluginManagerRemoveListener msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::View))
			return;

		PluginManagerListenerBridge::removeFromList(listeners, *this, client);
		break;
	}
	default:
		handleUnknownMessage("Plugin manager", client, message, messageTypeId);
		break;
	}
}