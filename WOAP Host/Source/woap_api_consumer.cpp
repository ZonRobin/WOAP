#include "woap_api_consumer.h"
#include "woap_api_core.h"
#include "woap_user.h"
#include "woap_api_messages_api_core.h"

using namespace WOAP;
using namespace WOAP::API;

APIConsumer::APIConsumer()
{
	apiCore = APICore::getInstance();
}

APIConsumer::~APIConsumer()
{

}

void APIConsumer::initializeAPIConsumer()
{
	apiCore->addListenerBatch(this, getMessageTypesToListenTo());
}

void APIConsumer::terminateAPIConsumer()
{
	apiCore->removeListenerBatch(this, getMessageTypesToListenTo());
}

void APIConsumer::handleUnknownMessage(const String& receiverName, OSC::RemoteClient&, const OSCMessage& message, int messageTypeId)
{
	ignoreUnused(receiverName, message, messageTypeId);
	DBG(receiverName + " received message " + 
		message.getAddressPattern().toString() + " along with typeId " + 
		String(messageTypeId) + " but it was left unproccessed"
	);
	jassertfalse;
}

void APIConsumer::handleUnauthorizedMessage(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message) const
{
	apiCore->handleUnauthorizedMessage(client, message);
}

void APIConsumer::handleInvalidMessage(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message) const
{
	apiCore->handleInvalidMessage(client, message);
}

bool APIConsumer::testValidity(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message) const
{
	if (!message.isValid())
	{
		handleInvalidMessage(client, message);
		return false;
	}

	return true;
}

bool APIConsumer::testPermissions(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message, uint32 permissionId) const
{
	if (!client.isAuthenticatedAndPermitted(permissionId))
	{
		handleUnauthorizedMessage(client, message);
		return false;
	}

	return true;
}

bool APIConsumer::testPermissions(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message, const Array<uint32>& permissionIds) const
{
	if (!client.isAuthenticatedAndPermitted(permissionIds))
	{
		handleUnauthorizedMessage(client, message);
		return false;
	}

	return true;
}

bool APIConsumer::testValidityAndPermissions(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message, uint32 permissionId) const
{
	if (!message.isValid())
	{
		handleInvalidMessage(client, message);
		return false;
	}

	if (!client.isAuthenticatedAndPermitted(permissionId))
	{
		handleUnauthorizedMessage(client, message);
		return false;
	}

	return true;
}

bool APIConsumer::testValidityAndPermissions(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message, const Array<uint32>& permissionIds) const
{
	if (!message.isValid())
	{
		handleInvalidMessage(client, message);
		return false;
	}

	if (!client.isAuthenticatedAndPermitted(permissionIds))
	{
		handleUnauthorizedMessage(client, message);
		return false;
	}

	return true;
}