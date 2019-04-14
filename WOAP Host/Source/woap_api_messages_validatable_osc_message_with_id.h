#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_api_messages_validatable_osc_message.h"

namespace WOAP
{
	namespace API
	{
		namespace Messages
		{
			class ValidatableOSCMessageWithId : public ValidatableOSCMessage
			{
			public:
				ValidatableOSCMessageWithId(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				virtual ~ValidatableOSCMessageWithId()
				{

				}

				inline void setMessageId(int newId)
				{
					jassert(newId > 0 && hasNumArguments(0));
					message->addInt32(newId);
				}

				inline bool isMessageWithId() const
				{
					return message->size() > 0 && message->begin()->isInt32();
				}

				inline static bool doesMessageHaveId(const OSCMessage& message)
				{
					return message.size() > 0 && message.begin()->isInt32();
				}

				/*
					Call doesMessageHaveId first
				*/
				inline static int getMessageId(const OSCMessage& message)
				{
					return message.begin()->getInt32();
				}

				inline int getMessageId() const
				{
					return message->begin()->getInt32();
				}

				inline virtual bool isValid() const override
				{
					if (!isMessageWithId() || !isAddressValid() || !areArgumentsValid() || !validate())
					{
						logMessageInvalidity();
						jassert(false);
						return false;
					}
					else
					{
						return true;
					}
				}

				inline virtual bool areArgumentsValid() const override
				{
					Array<OSCType> argumentTypes = getValidArgumentTypes();

					if (!hasNumArguments(argumentTypes.size()))
					{
						return false;
					}

					if (!isMessageWithId())
					{
						return false;
					}

					for (OSCType* type = argumentTypes.begin(); type != argumentTypes.end(); ++type)
					{
						if ((message->begin() + 1 + (type - argumentTypes.begin()))->getType() != *type)
						{
							return false;
						}
					}

					return true;
				}

				inline virtual OSCArgument& getArgument(int offset) const override
				{
					/* You're trying to get non-existing argument */
					jassert(getNumArguments() > offset);

					return *(message->begin() + offset + 1);
				}

				inline virtual bool hasNumArguments(int numArguments) const override
				{
					return (message->size() - 1) == numArguments;
				}

				inline virtual int getNumArguments() const override
				{
					return message->size() - 1;
				}

			protected:
				ValidatableOSCMessageWithId()
				{

				}

				inline void initializeMessage()
				{
					jassert(message == nullptr);
					message.setOwned(new OSCMessage(getValidAddress(), OSCArgument(getNextMessageId())));
				}

				template <typename Arg1, typename... Args>
				inline void initializeWithArgs(Arg1&& arg1, Args&&... args)
				{
					jassert(message == nullptr);
					message.setOwned(new OSCMessage(getValidAddress(), OSCArgument(getNextMessageId()), std::forward<Arg1>(arg1), std::forward<Args>(args)...));
				}

				inline void initializeResponseMessage(const ValidatableOSCMessageWithId& requestMessage)
				{
					jassert(message == nullptr);
					message.setOwned(new OSCMessage(getValidAddress(), OSCArgument(requestMessage.getMessageId())));
				}

				template <typename Arg1, typename... Args>
				inline void initializeResponseWithArgs(const ValidatableOSCMessageWithId& requestMessage, Arg1&& arg1, Args&&... args)
				{
					jassert(message == nullptr);
					message.setOwned(new OSCMessage(getValidAddress(), OSCArgument(requestMessage.getMessageId()), std::forward<Arg1>(arg1), std::forward<Args>(args)...));
				}

				inline static int getNextMessageId()
				{
					static int nextId = 1;
					return nextId++;
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ValidatableOSCMessageWithId)
			};
		}
	}
}
