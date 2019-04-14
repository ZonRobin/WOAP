#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace Sockets
	{
		class Message : public MemoryBlock
		{
		public:
			Message();
			Message(const void* dataToInitialiseFrom, size_t sizeInBytes);
			virtual ~Message();

		private:
			JUCE_LEAK_DETECTOR(Message)
		};

		class StringMessage : public Message
		{
		public:
			StringMessage(const CharPointer_UTF8& message);
			StringMessage(const CharPointer_UTF16& message);
			StringMessage(const CharPointer_UTF32& message);
			virtual ~StringMessage();
		};

		class BeaconMessage : public Sockets::Message
		{

		};
	}
}
