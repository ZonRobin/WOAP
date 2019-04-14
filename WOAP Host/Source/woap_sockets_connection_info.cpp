#include "woap_sockets_connection_info.h"

using namespace WOAP::Sockets;

ConnectionInfo::ConnectionInfo()
	: hostname(String()), port(0), timeout(0)
{

}

ConnectionInfo::ConnectionInfo(const String& host, const int portNumber, const int timeoutMsecs)
	: hostname(host), port(portNumber), timeout(timeoutMsecs)
{

}

ConnectionInfo::ConnectionInfo(const ConnectionInfo& other)
	: hostname(other.hostname), port(other.port), timeout(other.timeout)
{

}

ConnectionInfo::~ConnectionInfo()
{

}