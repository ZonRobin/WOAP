#include "woap_sockets_streaming_socket.h"

using namespace WOAP;

Sockets::StreamingSocket::StreamingSocket(
	const ConnectionInfo& connectionDetails,
	const String& threadName
)
	: Thread(threadName),
	connectionInfo(connectionDetails)
{
	streamingSocket = new juce::StreamingSocket();
}

Sockets::StreamingSocket::StreamingSocket(
	juce::StreamingSocket* socket,
	const String& threadName
)
	:	Thread(threadName),
	streamingSocket(socket)
{
	
}

Sockets::StreamingSocket::~StreamingSocket()
{
	streamingSocket->close();
	stopThread(5000);
}