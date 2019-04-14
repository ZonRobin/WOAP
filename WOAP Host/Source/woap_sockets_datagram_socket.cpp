#include "woap_sockets_datagram_socket.h"
#include "woap_api_osc_input_stream.h"
#include "woap_api_osc_output_stream.h"

using namespace WOAP;

Sockets::DatagramSocket::DatagramSocket(
	DatagramSocket::Listener& l,
	const DatagramSocket::ConnectionInfo& connectionDetails,
	const String& threadName
)
	: listener(l),
	Thread(threadName),
	receiverInfo(connectionDetails)
{
	datagramSocket = new juce::DatagramSocket();
	startThread();
}

Sockets::DatagramSocket::DatagramSocket(
	DatagramSocket::Listener& l,
	juce::DatagramSocket* socket,
	const String& threadName
)
	: listener(l),
	Thread(threadName),
	datagramSocket(socket)
{

}

Sockets::DatagramSocket::~DatagramSocket()
{
	datagramSocket->shutdown();
	stopThread(5000);
}

const Sockets::DatagramSocket::ConnectionInfo& Sockets::DatagramSocket::getConnectionInfo() const
{
	return receiverInfo;
}

void Sockets::DatagramSocket::addMessageToQueue(Sockets::Message* message)
{
	const ScopedLock l(messageQueue.getLock());
	messageQueue.add(message);
}

int Sockets::DatagramSocket::bind()
{
	return bind(receiverInfo.port, receiverInfo.host);
}

int Sockets::DatagramSocket::bind(int portNumber, const String& address)
{
	if (datagramSocket->bindToPort(portNumber, address))
	{
		DBG("Socket client connected to " + String(datagramSocket->getBoundPort()));
	}
	else
	{
		DBG("Datagram socket client not connected to");
	}
	
	return datagramSocket->getBoundPort();
}

void Sockets::DatagramSocket::run()
{
	while (true)
	{
		switch (datagramSocket->waitUntilReady(false, 33))
		{
		case -1:
			return;
		case 0:
			if (threadShouldExit())
				return;
			break;
		case 1:
			if (!writeMessages())
				return;
			break;
		}

		switch (datagramSocket->waitUntilReady(true, 33))
		{
		case -1:
			return;
		case 0:
			if (threadShouldExit())
				return;
			break;
		case 1:
			if (!readMessages())
				return;
			break;
		}
	}
}


bool Sockets::DatagramSocket::writeMessages()
{
	if (messageQueue.isEmpty())
		return true;

	const ScopedLock l(messageQueue.getLock());
	
	for (Sockets::Message* message : messageQueue)
	{		
		if (datagramSocket->write(receiverInfo.host, receiverInfo.port, message->getData(), (int)message->getSize()) == -1)
			return false;
	}

	messageQueue.clearQuick(true);
	return true;
}

void Sockets::DatagramSocket::sendBeaconMessage()
{
	addMessageToQueue(new BeaconMessage());
}


bool Sockets::DatagramSocket::readMessages()
{
	const int numBytesRead = datagramSocket->read(bufferRaw, bufferRawSize * sizeof(char), false);
	if (numBytesRead == -1)
		return false;

	if (numBytesRead == 0)
		return true;

	lastMessageReceivedTime = Time::getMillisecondCounter();
	listener.messageReceived(*this, &bufferRaw, numBytesRead);	

	return true;
}

void Sockets::DatagramSocket::sendOSCMessage(const OSCMessage& message)
{
	API::OSC::OutputStream outStream;
	outStream.writeMessage(message);

	addMessageToQueue(new Sockets::Message(outStream.getData(), outStream.getDataSize()));
}

void Sockets::DatagramSocket::sendOSCBundle(const OSCBundle& bundle)
{
	API::OSC::OutputStream outStream;
	outStream.writeBundle(bundle);

	addMessageToQueue(new Sockets::Message(outStream.getData(), outStream.getDataSize()));
}

void Sockets::DatagramSocket::sendRawMessage(Sockets::Message* message)
{
	addMessageToQueue(message);
}