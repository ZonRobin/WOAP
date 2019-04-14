#include "woap_sockets_streaming_socket_client.h"
#include "woap_sockets_message.h"
#include "woap_api_osc_input_stream.h"
#include "woap_api_osc_output_stream.h"

using namespace WOAP;

Sockets::StreamingSocketClient::StreamingSocketClient(
	ConnectionListener* listener, 
	const ConnectionInfo& connectionDetails, 
	const String& threadName
)
	: connectionListener(listener), Sockets::StreamingSocket(connectionDetails, threadName)
{
	memset(bufferRaw, 0, bufferRawSize * sizeof(char));
}

Sockets::StreamingSocketClient::StreamingSocketClient(
	ConnectionListener* listener, 
	juce::StreamingSocket* socket, 
	const String& threadName
)
	: connectionListener(listener), Sockets::StreamingSocket(socket, threadName)
{
	memset(bufferRaw, 0, bufferRawSize * sizeof(char));
	setAutoConnect(false);
	startThread();
}

Sockets::StreamingSocketClient::~StreamingSocketClient()
{
	signalThreadShouldExit();
	streamingSocket->close();
	stopThread(5000);
}

void Sockets::StreamingSocketClient::addMessageToQueue(Sockets::Message* message)
{
	messageQueue.add(message);
}

bool Sockets::StreamingSocketClient::connect()
{
	if (streamingSocket->connect(connectionInfo.hostname, connectionInfo.port, connectionInfo.timeout))
	{
		DBG("Socket client connected to " + String(connectionInfo.port));
		connectionListener->connected(this);
		return true;
	}
	else
	{
		DBG("Socket client not connected to " + String(connectionInfo.port));
		return false;
	}
}

bool Sockets::StreamingSocketClient::runConnectionLoop(bool forceReconnect)
{
	if (forceReconnect)
		streamingSocket->close();

	while (true)
	{
		if (threadShouldExit())
			return false;

		if (connect())
		{
			lastMessageReceivedTime = Time::getMillisecondCounter();
			return true;
		}
	}
}

void Sockets::StreamingSocketClient::notifyConnectionClosed()
{
	connectionListener->connectionClosed(this);
}

void Sockets::StreamingSocketClient::run()
{
	while (true)
	{
		if (autoConnect && (lastMessageReceivedTime + 2000 < Time::getMillisecondCounter()))
		{
			if (!runConnectionLoop())
				return;
		}

		switch (streamingSocket->waitUntilReady(false, 33))
		{
		case -1:
			if (autoConnect)
			{
				if (!runConnectionLoop(true)) { return; }
				break;
			}
			else
			{
				notifyConnectionClosed(); return;
			}
		case 0:
			if (threadShouldExit()) { return; }
			break;
		case 1:
			if (!writeMessages()) { notifyConnectionClosed(); return; }
			break;
		}

		switch (streamingSocket->waitUntilReady(true, 33))
		{
		case -1:
			if (autoConnect)
			{
				if (!runConnectionLoop(true)) { return; }
				break;
			}
			else
			{
				notifyConnectionClosed(); return;
			}
		case 0:
			if (threadShouldExit()) { return; }
			break;
		case 1:
			if (!readMessages()) { notifyConnectionClosed(); return; }
			break;
		}
	}
}


bool Sockets::StreamingSocketClient::writeMessages()
{
	if (messageQueue.isEmpty())
		return true;

	const ScopedLock l(messageQueue.getLock());

	for (Sockets::Message* message : messageQueue)
	{
		int32 messageHeader = { ByteOrder::swapIfBigEndian((int32)message->getSize()) };

		MemoryBlock messageData(sizeof(messageHeader) + message->getSize());
		messageData.copyFrom(&messageHeader, 0, sizeof(messageHeader));
		messageData.copyFrom(message->getData(), sizeof(messageHeader), message->getSize());

		if (streamingSocket->write(messageData.getData(), (int)messageData.getSize()) == -1)
		{
			return false;
		}
	}

	messageQueue.clearQuick(true);
	return true;
}

bool Sockets::StreamingSocketClient::readMessages()
{
	const int numBytesRead = streamingSocket->read(bufferRaw, bufferRawSize * sizeof(char), false);
	if (numBytesRead == -1)
		return false;

	if (numBytesRead == 0)
		return true;

	lastMessageReceivedTime = Time::getMillisecondCounter();
	for (int i = 0; i < numBytesRead;)
	{
		const int32 messageLength = (int)ByteOrder::swapIfBigEndian((int32)(bufferRaw[i]));
		if (messageLength > numBytesRead || messageLength == 0)
		{
			DBG("Error in parsing message length for message " + Base64::toBase64(&bufferRaw[i], 50));
			return false;
		}

		listeners.call(&Listener::messageReceived, this, &bufferRaw[i + sizeof(int32)], messageLength);
		i += sizeof(int32) + messageLength;
	}

	return true;
}

void Sockets::StreamingSocketClient::sendOSCMessage(const OSCMessage& message)
{
	API::OSC::OutputStream outStream;
	outStream.writeMessage(message);

	addMessageToQueue(new Sockets::Message(outStream.getData(), outStream.getDataSize()));
}

void Sockets::StreamingSocketClient::sendOSCBundle(const OSCBundle& bundle)
{
	API::OSC::OutputStream outStream;
	outStream.writeBundle(bundle);

	addMessageToQueue(new Sockets::Message(outStream.getData(), outStream.getDataSize()));
}

void Sockets::StreamingSocketClient::sendRawMessage(Sockets::Message* message)
{
	addMessageToQueue(message);
}