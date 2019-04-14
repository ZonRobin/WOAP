#include "woap_sockets_streaming_socket_server.h"
#include "woap_sockets_message.h"

using namespace WOAP;

Sockets::StreamingSocketServer::StreamingSocketServer(const ConnectionInfo& connectionDetails, const String& threadName)
	: Sockets::StreamingSocket(connectionDetails, threadName)
{
	
}

Sockets::StreamingSocketServer::~StreamingSocketServer()
{
	stop(5000);
}

void Sockets::StreamingSocketServer::stop(int timeoutMs)
{
	signalThreadShouldExit();
	streamingSocket->close();
	stopThread(timeoutMs);
}

bool Sockets::StreamingSocketServer::runConnectionLoop(bool forceReconnect)
{
	if (forceReconnect)
		streamingSocket->close();

	while (true)
	{
		if (threadShouldExit())
			return false;

		if (connect())
			return true;
	}
}

bool Sockets::StreamingSocketServer::connect()
{	
	if (streamingSocket->createListener(connectionInfo.port, connectionInfo.hostname))
	{
		DBG("Socket server bound to " + String(connectionInfo.port));
		return true;
	}
	else
	{
		DBG("Socket server not bound to " + String(connectionInfo.port));
		Time::waitForMillisecondCounter(Time::getMillisecondCounter() + connectionInfo.timeout);
		return false;
	}
}

void Sockets::StreamingSocketServer::connectionClosed(Sockets::StreamingSocketClient* client)
{
	MessageManager::callAsync([=]() {
		ScopedPointer<Sockets::StreamingSocketClient> bye(client);
		clients.removeObject(client, false);
		listeners.call(&Listener::clientDisconnected, this, client);
	});
}

void Sockets::StreamingSocketServer::run()
{
	runConnectionLoop();

	while (!threadShouldExit())
	{
		juce::StreamingSocket* newClient = streamingSocket->waitForNextConnection();
		if (newClient != nullptr)
		{
			StreamingSocketClient* client = new StreamingSocketClient(this, newClient);
			client->setAutoConnect(false);
			clients.add(client);
			
			listeners.call(&Listener::clientConnected, this, client);
		}
	}
}
