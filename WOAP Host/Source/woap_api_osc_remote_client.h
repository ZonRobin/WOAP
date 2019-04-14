#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	class User;
	class UserLogin;

	namespace Sockets
	{
		class Message;
	}

	namespace API
	{
		namespace ListenerBridges
		{
			class IListenerBridge;
		}

		namespace OSC
		{
			class RemoteClient : public Timer
			{
			public:
				RemoteClient();
				virtual ~RemoteClient();

				virtual void sendOSCMessage(const OSCMessage& message) = 0;
				virtual void sendOSCBundle(const OSCBundle& bundle) = 0;
				virtual void sendRawMessage(Sockets::Message* message) = 0;

				virtual void timerCallback();

				void addListenerBridge(ListenerBridges::IListenerBridge* newBridge);
				void removeListenerBridge(ListenerBridges::IListenerBridge* oldBridge);
				void removeAllListenerBridges();

				User* getUser() const { return user; }
				void setUser(User* newUser);

				User* getOwnerUser() const { return ownerUser; }
				void setOwnerUser(User* newUser) { ownerUser = newUser; }

				void authenticate();

				void setLogin(UserLogin* login) { activeLogin = login; }
				bool isAuthenticated() const;
				bool isAuthenticatedAndPermitted(const Array<uint32>& permissionIds) const;
				bool isAnonymous() const;

				UserLogin* getActiveLogin() const { return activeLogin; }

			protected:
				User* user;
				User* ownerUser;
				UserLogin* activeLogin;

				OwnedArray<ListenerBridges::IListenerBridge> listenerBridges;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RemoteClient)
			};
		}	
	}
}
