#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace API
	{
		namespace OSC
		{
			class RemoteClient;

			class MessageReceiver
			{
			public:
				virtual void messageReceived(RemoteClient& client, const OSCMessage& message, int messageTypeId) = 0;
			};
		}
	}
}
