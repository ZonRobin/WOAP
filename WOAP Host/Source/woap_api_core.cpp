#include "woap_api_core.h"
#include "woap_api_osc_input_stream.h"
#include "woap_api_osc_output_stream.h"
#include "woap_api_listener_bridges_base.h"
#include "woap_api_osc_remote_client.h"
#include "woap_api_messages_api_core.h"
#include "woap_access_control_core.h"
#include "woap_user_login.h"
#include "woap_user.h"
#include "woap_main.h"
#include "woap_configuration_manager.h"

using namespace WOAP;
using namespace WOAP::API;
using namespace Messages;

SINGLETON_IMPLEMENTATION(APICore)

APICore::APICore()
{	
	addListenerBatch(this, {
		API::AddressWithTypeId(ApiCoreGetStatus::getAddress(), MainControlMessage::GetStatus),
		API::AddressWithTypeId(ApiCoreGetServerInfo::getAddress(), MainControlMessage::GetServerInfo),
		API::AddressWithTypeId(ApiCoreAuthenticate::getAddress(), MainControlMessage::Authenticate),
		API::AddressWithTypeId(ApiCoreLogin::getAddress(), MainControlMessage::Login),
	});
	
	mainControl = new Sockets::StreamingSocketServer(Sockets::ConnectionInfo(String("127.0.0.1"), 5020, 5000), "Main control worker");
	mainControl->addListener(this);
	mainControl->startThread();
}

APICore::~APICore()
{

}

void APICore::shutdown()
{
	const ScopedLock sl(mainControl->getClients().getLock());
	for (Sockets::StreamingSocketClient* client : mainControl->getClients())
	{
		client->removeListener(this);
	}

	mainControl = nullptr;
}

void APICore::clientConnected(Sockets::StreamingSocketServer*, Sockets::StreamingSocketClient* client)
{
	if (ConfigurationManager::getInstance()->isAnonymousLoginEnabled())
	{
		client->setUser(AccessControlCore::getInstance()->getAnonymousUserOrNull());
		client->authenticate();
	}

	client->addListener(this);
	DBG("API client connected");
}

void APICore::clientDisconnected(Sockets::StreamingSocketServer*, Sockets::StreamingSocketClient* client)
{
	client->removeListener(this);
	DBG("API client left");
}

void APICore::messageReceived(Sockets::StreamingSocketClient* client, const void* messageData, size_t messageLength)
{
	OSC::InputStream inStream(messageData, messageLength);
	
	try
	{
		OSCBundle::Element content = inStream.readElementWithKnownSize(messageLength);

		MessageManager::callAsync([this, client, content]() {
			dispatchMessageElementToListeners(*client, content);
		});
	}
	catch (OSCFormatError er) 
	{ 
		client->sendOSCMessage(ApiCoreMessageRejected(er.what(), ApiCoreMessageRejected::Reason::ParsingError));
		DBG("OSC parsing error: " + String(er.what())); 
	}
}

void APICore::dispatchMessageElementToListeners(OSC::RemoteClient& client, const OSCBundle::Element& element)
{
	if (element.isMessage())
	{
		if (nullptr != client.getActiveLogin()) 
			client.getActiveLogin()->refresh();

		if (messageReceivers.messageReceived(element.getMessage(), client) == 0)
			DBG("API message received but no listener accepted it: " + element.getMessage().getAddressPattern().toString());
		else
			DBG("API message: " + element.getMessage().getAddressPattern().toString());
	}
	else
	{
		for (OSCBundle::Element& subElement : element.getBundle())
			dispatchMessageElementToListeners(client, subElement);
	}
}


void APICore::addListener(const String& method, OSC::MessageReceiver* receiver, int messageTypeId)
{
	messageReceivers.addListener(method, receiver, messageTypeId);
}

void APICore::addListenerBatch(OSC::MessageReceiver* receiver, Array<AddressWithTypeId> addressWithIds)
{
	for (AddressWithTypeId& addr : addressWithIds)
		addListener(addr.address, receiver, addr.typeId);
}

void APICore::removeListener(const String& method, OSC::MessageReceiver* receiver)
{
	messageReceivers.removeListener(method, receiver);
}

void APICore::removeListenerBatch(OSC::MessageReceiver* receiver, Array<AddressWithTypeId> addressWithIds)
{
	for (AddressWithTypeId& addr : addressWithIds)
		removeListener(addr.address, receiver);
}

void APICore::messageReceived(OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId)
{
	switch (messageTypeId)
	{
	case MainControlMessage::GetStatus:
	{
		Messages::ApiCoreGetStatus msg(message);

		if (!msg.isValid())
			return;

		client.sendOSCMessage(Messages::ApiCoreReady());
		break;
	}
	case MainControlMessage::GetServerInfo:
	{
		Messages::ApiCoreGetServerInfo msg(message);

		if (!msg.isValid())
			return;

		Main* main = Main::getInstanceCast();

		client.sendOSCMessage(Messages::ApiCoreServerInfo(
			msg,
			main->getDeviceVersion(),
			main->getDeviceId(),
			main->getProcessId(), 
			ConfigurationManager::getInstance()->isAnonymousLoginEnabled(),
			client.isAuthenticated()
		));
		break;
	}
	case MainControlMessage::Authenticate:
	{
		ApiCoreAuthenticate msg(message);

		if (!msg.isValid())
			return;

		UserLogin* login = AccessControlCore::getInstance()->findLoginByLogTag(msg.getLogTag());
		if (login == nullptr)
		{
			client.sendOSCMessage(ApiCoreAuthenticationResponse(msg, ApiCoreAuthenticationResponse::LogTagUnknownOrExpired));
			return;
		}

		client.setUser(login->getUser());
		client.setLogin(login);

		client.sendOSCMessage(ApiCoreAuthenticationResponse(msg, ApiCoreAuthenticationResponse::Ok));
		break;
	}
	case MainControlMessage::Login:
	{
		ApiCoreLogin msg(message);
		
		if (!msg.isValid())
			return;

		User* user = AccessControlCore::getInstance()->findUserWithPassword(msg.getPassword());
		if (user == nullptr)
		{
			client.sendOSCMessage(ApiCoreLoginResponse(msg, ApiCoreLoginResponse::ResponseCode::WrongCredentials));
			return;
		}

		client.setUser(user);
		client.authenticate();

		client.sendOSCMessage(ApiCoreLoginResponse(msg, ApiCoreLoginResponse::ResponseCode::Ok, client.getActiveLogin()->getLogTag()));
		break;
	}
	default:
		break;
	}
}

void APICore::handleUnauthorizedMessage(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message) const
{
	client.sendOSCMessage(ApiCoreMessageRejected(message, ApiCoreMessageRejected::Reason::Unauthorized));
}

void APICore::handleInvalidMessage(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message) const
{
	client.sendOSCMessage(ApiCoreMessageRejected(message, ApiCoreMessageRejected::Reason::InvalidMessage));
}