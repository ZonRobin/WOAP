#include "woap_api_osc_listener_library.h"
#include "woap_api_osc_message_receiver.h"
#include "woap_api_osc_remote_client.h"

using namespace WOAP::API::OSC;

MessageReceiverWrapper::MessageReceiverWrapper(MessageReceiver* MessageReceiver, int messageId)
	: receiver(MessageReceiver), messageType(messageId)
{

}

bool MessageReceiverWrapper::operator==(const MessageReceiverWrapper& other)
{
	return receiver == other.receiver;
}

bool MessageReceiverWrapper::operator!=(const MessageReceiverWrapper& other)
{
	return !(*this == other);
}


MessageReceiverLibrary::MessageReceiverLibrary()
{

}

MessageReceiverLibrary::~MessageReceiverLibrary()
{

}

void MessageReceiverLibrary::addListener(const String& method, MessageReceiver* receiver, int messageTypeId)
{
	ListenerLibrary::addListener(method, new MessageReceiverWrapper(receiver, messageTypeId));
}

void MessageReceiverLibrary::removeListener(const String& method, MessageReceiver* receiver)
{
	OwnedArray<MessageReceiverWrapper>* listeners = getListenersOrNull(method);

	if (listeners != nullptr)
	{
		MessageReceiverWrapper itemToRemove(receiver);

		for (MessageReceiverWrapper** item = listeners->begin(); item != listeners->end(); ++item)
		{
			if (**item == itemToRemove)
			{
				removeFromArray(*listeners, item - listeners->begin(), true);
				return;
			}
		}
	}
}

int MessageReceiverLibrary::messageReceived(const OSCMessage& message, RemoteClient& client)
{
	OwnedArray<MessageReceiverWrapper>* list = getListenersOrNull(message.getAddressPattern().toString());

	if (list == nullptr)
		return 0;

	for (MessageReceiverWrapper* item : *list)
		item->receiver->messageReceived(client, message, item->messageType);

	return list->size();
}