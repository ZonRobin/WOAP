#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_sockets_streaming_socket.h"
#include "woap_api_osc_remote_client.h"

namespace WOAP
{
	namespace Sockets
	{
		class Message;

		class StreamingSocketClient : public Sockets::StreamingSocket, public API::OSC::RemoteClient
		{
		public:
			class Listener
			{
			public:
				virtual void messageReceived(Sockets::StreamingSocketClient* client, const void* messageData, size_t messageLength) = 0;
			};

			class ConnectionListener
			{
			public:
				virtual void connected(Sockets::StreamingSocketClient* client) = 0;
				virtual void connectionClosed(Sockets::StreamingSocketClient* client) = 0;
			};

			StreamingSocketClient(
				ConnectionListener* listener,
				const ConnectionInfo& connectionDetails,
				const String& threadName = String("Socket client")
			);

			StreamingSocketClient(
				ConnectionListener* listener,
				juce::StreamingSocket* socket,
				const String& threadName = String("Socket client")
			);

			virtual ~StreamingSocketClient();

			virtual void sendOSCMessage(const OSCMessage& message) override;
			virtual void sendOSCBundle(const OSCBundle& bundle) override;
			virtual void sendRawMessage(Sockets::Message* message) override;

			void addListener(Listener* listener) { listeners.add(listener); }
			void removeListener(Listener* listener) { listeners.remove(listener); }

			void setAutoConnect(bool connect) { autoConnect = connect; }

		protected:
			virtual void run() override;

			void addMessageToQueue(Sockets::Message* message);

			bool connect();
			bool runConnectionLoop(bool forceReconnect = false);
			void notifyConnectionClosed();

			bool writeMessages();
			bool readMessages();

			OwnedArray<Sockets::Message, CriticalSection> messageQueue;

			ListenerList<StreamingSocketClient::Listener> listeners;
			StreamingSocketClient::ConnectionListener* connectionListener;

			bool autoConnect = true;

			static const size_t bufferRawSize = 1024 * 1024 * 2;
			char bufferRaw[bufferRawSize];

			uint32 lastMessageReceivedTime = 0;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StreamingSocketClient)
		};
	}
}

