#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_sockets_connection_info.h"

namespace WOAP
{	
	namespace Sockets
	{
		class StreamingSocket : public Thread
		{
		public:
			StreamingSocket(
				const ConnectionInfo& connectionDetails,
				const String& threadName
			);

			StreamingSocket(
				juce::StreamingSocket* socket,
				const String& threadName
			);
			
			virtual ~StreamingSocket();

		protected:
			ScopedPointer<juce::StreamingSocket> streamingSocket;
			const ConnectionInfo connectionInfo;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sockets::StreamingSocket)
		};
	}
	
}
