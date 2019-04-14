#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace Sockets
	{
		struct ConnectionInfo
		{
			const String hostname;
			const int port;
			const int timeout;

			ConnectionInfo();
			ConnectionInfo(const String& host, const int portNumber, const int timeoutMsecs);
			ConnectionInfo(const ConnectionInfo& other);
			virtual ~ConnectionInfo();

		private:
			JUCE_LEAK_DETECTOR(ConnectionInfo)
		};
	}
}
