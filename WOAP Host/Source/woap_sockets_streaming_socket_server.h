#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_sockets_streaming_socket.h"
#include "woap_sockets_streaming_socket_client.h"

namespace WOAP
{
	namespace Sockets
	{
		class StreamingSocketServer : public Sockets::StreamingSocket, public StreamingSocketClient::ConnectionListener
		{
		public:
			class Listener
			{
			public:
				virtual void clientConnected(Sockets::StreamingSocketServer* server, Sockets::StreamingSocketClient* client) = 0;
				virtual void clientDisconnected(Sockets::StreamingSocketServer* server, Sockets::StreamingSocketClient* client) = 0;
			};

			StreamingSocketServer(
				const ConnectionInfo& connectionDetails,
				const String& threadName = String("Socket server")
			);

			virtual ~StreamingSocketServer();
			void stop(int timeoutMs);

			virtual void connected(Sockets::StreamingSocketClient*) override {}
			virtual void connectionClosed(Sockets::StreamingSocketClient* client) override;

			const OwnedArray<StreamingSocketClient, CriticalSection>& getClients() const { return clients; }

			void addListener(Listener* listener) { listeners.add(listener); }
			void removeListener(Listener* listener) { listeners.remove(listener); }

		protected:
			virtual void run() override;
			bool connect();
			bool runConnectionLoop(bool forceReconnect = false);

			OwnedArray<StreamingSocketClient, CriticalSection> clients;
			ListenerList<StreamingSocketServer::Listener> listeners;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StreamingSocketServer)
		};
	}

}
