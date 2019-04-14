#include "woap_api_osc_remote_client.h"
#include "woap_api_listener_bridges_base.h"
#include "woap_api_messages_api_core.h"
#include "woap_user_login.h"
#include "woap_user.h"
#include "woap_access_control_core.h"

using namespace WOAP;
using namespace WOAP::API;
using namespace WOAP::API::OSC;

RemoteClient::RemoteClient()
	: user(nullptr), ownerUser(nullptr), activeLogin(nullptr)
{
	startTimer(5000); /* Heartbeat timer */
}

RemoteClient::~RemoteClient()
{

}

void RemoteClient::timerCallback()
{
	sendOSCMessage(Messages::ApiCoreHeartbeat());
}

void RemoteClient::addListenerBridge(ListenerBridges::IListenerBridge* newBridge)
{
	listenerBridges.add(newBridge);
}

void RemoteClient::removeListenerBridge(ListenerBridges::IListenerBridge* oldBridge)
{
	listenerBridges.removeObject(oldBridge);
}

void RemoteClient::removeAllListenerBridges()
{
	listenerBridges.clearQuick(true);
}

void RemoteClient::setUser(User* newUser)
{
	/* You should never change user once it's set */
	jassert(user == nullptr || user->isAnonymous());

	user = newUser;
}

void RemoteClient::authenticate()
{
	/* Cant authenticate null user */
	jassert(user != nullptr);

	activeLogin = user->registerLogin();
}

bool RemoteClient::isAuthenticated() const
{
	return user != nullptr && activeLogin != nullptr && activeLogin->isActive();
}

bool RemoteClient::isAuthenticatedAndPermitted(const Array<uint32>& permissionIds) const
{
	return isAuthenticated() && user->hasPermissions(permissionIds);
}

bool RemoteClient::isAnonymous() const
{
	return user != nullptr && user->isAnonymous();
}