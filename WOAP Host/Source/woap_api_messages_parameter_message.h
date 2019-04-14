#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_api_messages_validatable_osc_message.h"
#include "woap_functions.h"

namespace WOAP
{
	namespace API
	{
		namespace Messages
		{
			class ParameterBoolMessage : public ValidatableOSCMessage
			{
			public:
				ParameterBoolMessage(const OSCMessage& msg, const Identifiable* owner)
					: ownerId(owner->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				ParameterBoolMessage(const Identifiable* owner)
					: ownerId(owner->getId().toDashedString())
				{
					
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32		/* NewState */
					});
				}

				inline bool getNewState() const
				{
					return getArgumentBool(0);
				}

			protected:
				String ownerId;

				void initializeMessage(bool newState)
				{
					ValidatableOSCMessage::initializeWithArgs(OSCArgument(boolToInt(newState)));
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterBoolMessage)
			};

			class ParameterIntMessage : public ValidatableOSCMessage
			{
			public:
				ParameterIntMessage(const OSCMessage& msg, const Identifiable* owner)
					: ownerId(owner->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				ParameterIntMessage(const Identifiable* owner)
					: ownerId(owner->getId().toDashedString())
				{

				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32		/* NewValue */
					});
				}

				inline int getNewValue() const
				{
					return getArgumentInt(0);
				}

			protected:
				String ownerId; 
				
				void initializeMessage(int newState)
				{
					ValidatableOSCMessage::initializeWithArgs(OSCArgument(newState));
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterIntMessage)
			};

			class ParameterFloatMessage : public ValidatableOSCMessage
			{
			public:
				ParameterFloatMessage(const OSCMessage& msg, const Identifiable* owner)
					: ownerId(owner->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				ParameterFloatMessage(const Identifiable* owner)
					: ownerId(owner->getId().toDashedString())
				{

				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::float32		/* NewValue */
					});
				}

				inline float getNewValue() const
				{
					return getArgumentFloat(0);
				}

			protected:
				String ownerId;

				void initializeMessage(float newState)
				{
					ValidatableOSCMessage::initializeWithArgs(OSCArgument(newState));
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterFloatMessage)
			};
		}
	}
}
