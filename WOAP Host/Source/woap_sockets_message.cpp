#include "woap_sockets_message.h"

using namespace WOAP;

Sockets::Message::Message()
{

}

Sockets::Message::Message(const void* dataToInitialiseFrom, size_t sizeInBytes)
	: MemoryBlock(dataToInitialiseFrom, sizeInBytes)
{

}

Sockets::Message::~Message()
{

}

Sockets::StringMessage::StringMessage(const CharPointer_UTF8& message)
	: Message(message, message.sizeInBytes())
{

}

Sockets::StringMessage::StringMessage(const CharPointer_UTF16& message)
	: Message(message, message.sizeInBytes())
{

}

Sockets::StringMessage::StringMessage(const CharPointer_UTF32& message)
	: Message(message, message.sizeInBytes())
{

}

Sockets::StringMessage::~StringMessage()
{

}