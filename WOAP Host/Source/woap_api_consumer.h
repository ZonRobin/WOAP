#pragma once

#include "woap_api_osc_message_receiver.h"
#include "woap_api_imessage.h"

using std::initializer_list;

namespace WOAP
{
	namespace API
	{
		namespace Messages
		{
			class ValidatableOSCMessage;
		}

		class APICore;

		class APIConsumer : public API::OSC::MessageReceiver
		{
		public:
			APIConsumer();
			virtual ~APIConsumer();

			void initializeAPIConsumer();
			void terminateAPIConsumer();

			virtual Array <API::AddressWithTypeId> getMessageTypesToListenTo() const = 0;
			virtual void messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId) = 0;

			void handleUnknownMessage(const String& receiverName, OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId);
			void handleUnauthorizedMessage(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message) const;
			void handleInvalidMessage(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message) const;

			bool testValidity(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message) const;

			bool testPermissions(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message, uint32 permissionId) const;
			bool testPermissions(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message, const Array<uint32>& permissionIds) const;
			
			bool testValidityAndPermissions(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message, uint32 permissionId) const;
			bool testValidityAndPermissions(OSC::RemoteClient& client, const Messages::ValidatableOSCMessage& message, const Array<uint32>& permissionIds) const;

		protected:
			APICore* getApiCore() const { return apiCore; }

		private:
			APICore* apiCore;
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(APIConsumer)
		};
	}
}
