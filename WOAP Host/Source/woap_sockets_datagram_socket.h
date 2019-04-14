#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_api_osc_remote_client.h"
#include "woap_sockets_message.h"

namespace WOAP
{
	namespace Sockets
	{
		class DatagramSocket : public Thread, public API::OSC::RemoteClient
		{
		public:
			class Listener
			{
			public:
				virtual void messageReceived(Sockets::DatagramSocket& client, const void* messageData, size_t messageLength) = 0;
			};

			struct ConnectionInfo
			{
				int port;
				String host;

				ConnectionInfo(int portNumber = 0, const String& hostName = String())
					: port(portNumber), host(hostName)
				{
					
				}
			};

			DatagramSocket(
				DatagramSocket::Listener& listener,
				const DatagramSocket::ConnectionInfo& receiverInfo,
				const String& threadName = "Datagram socket"
			);

			DatagramSocket(
				DatagramSocket::Listener& listener,
				juce::DatagramSocket* socket,
				const String& threadName = "Datagram socket"
			);

			virtual ~DatagramSocket();

			const ConnectionInfo& getConnectionInfo() const;

			virtual void sendOSCMessage(const OSCMessage& message) override;
			virtual void sendOSCBundle(const OSCBundle& bundle) override;
			virtual void sendRawMessage(Sockets::Message* message) override;

			int bind();
			int bind(int portNumber = 0, const String& address = String());

		protected:
			virtual void run() override;

			void addMessageToQueue(Sockets::Message* message);

			bool writeMessages();

			void sendBeaconMessage();

			bool readMessages();

			ScopedPointer<juce::DatagramSocket> datagramSocket;
			const ConnectionInfo receiverInfo;

			OwnedArray<Sockets::Message, CriticalSection> messageQueue;

			DatagramSocket::Listener& listener;

			static const size_t bufferRawSize = 10000;
			char bufferRaw[bufferRawSize];

			uint32 lastMessageReceivedTime = 0;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sockets::DatagramSocket)
		};
	}
}
