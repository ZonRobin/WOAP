#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_api_osc_listener_library.h"
#include "woap_sockets_streaming_socket_server.h"
#include "woap_sockets_streaming_socket_client.h"
#include "woap_api_osc_message_receiver.h"
#include "woap_api_imessage.h"
#include <map>
#include "woap_singleton.h"

using std::map;

namespace WOAP
{
	namespace API
	{
		namespace OSC
		{
			class RemoteClient;
		}

		namespace Messages
		{
			class ValidatableOSCMessage;
		}

		class APICore :
			public Sockets::StreamingSocketServer::Listener, 
			public Sockets::StreamingSocketClient::Listener, 
			public OSC::MessageReceiver
		{
		public:
			enum MainControlMessage
			{
				GetStatus = 0,
				GetServerInfo,
				Authenticate,
				Login
			};

			void shutdown();

			virtual void clientConnected(Sockets::StreamingSocketServer* server, Sockets::StreamingSocketClient* client) override;
			virtual void clientDisconnected(Sockets::StreamingSocketServer*, Sockets::StreamingSocketClient*) override;

			virtual void messageReceived(Sockets::StreamingSocketClient* client, const void* messageData, size_t messageLength) override;

			void addListener(const String& method, OSC::MessageReceiver* receiver, int messageTypeId = 0);
			void addListenerBatch(OSC::MessageReceiver* receiver, Array<AddressWithTypeId> addressWithIds);

			void removeListener(const String& method, OSC::MessageReceiver* receiver);
			void removeListenerBatch(OSC::MessageReceiver* receiver, Array<AddressWithTypeId> addressWithIds);

			virtual void messageReceived(OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId) override;

			void handleUnauthorizedMessage(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message) const;
			void handleInvalidMessage(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message) const;

		protected:
			void dispatchMessageElementToListeners(OSC::RemoteClient& client, const OSCBundle::Element& element);

			ScopedPointer<Sockets::StreamingSocketServer> mainControl;
			OSC::MessageReceiverLibrary messageReceivers;

		private:
			SINGLETON(APICore)
		};
	}
	
}
