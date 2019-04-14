#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_listener_library.h"

namespace WOAP
{
	namespace API
	{
		namespace OSC
		{
			class MessageReceiver;
			class RemoteClient;

			class MessageReceiverWrapper
			{
			public:
				MessageReceiverWrapper(MessageReceiver* MessageReceiver, int messageId = 0);

				bool operator==(const MessageReceiverWrapper& other);
				bool operator!=(const MessageReceiverWrapper& other);

			protected:
				MessageReceiver* receiver;
				int messageType;

				friend class MessageReceiverLibrary;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageReceiverWrapper)
			};

			class MessageReceiverLibrary : protected ListenerLibrary<MessageReceiverWrapper, OwnedArray<MessageReceiverWrapper>>
			{
			public:
				MessageReceiverLibrary();
				virtual ~MessageReceiverLibrary();

				void addListener(const String& method, MessageReceiver* receiver, int messageTypeId = 0);
				void removeListener(const String& method, MessageReceiver* receiver);

				int messageReceived(const OSCMessage& message, RemoteClient& client);

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MessageReceiverLibrary)
			};
		}
	}
}
