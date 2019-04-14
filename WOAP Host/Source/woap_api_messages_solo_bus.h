#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_api_messages_validatable_osc_message.h"
#include "woap_api_messages_validatable_osc_message_with_id.h"

namespace WOAP
{
	namespace API
	{
		namespace Messages
		{
			class SoloBusAddAudioDataListener : public ValidatableOSCMessage
			{
			public:
				SoloBusAddAudioDataListener(const OSCMessage& msg) 
					: ValidatableOSCMessage(msg)
				{

				}

				SoloBusAddAudioDataListener(int minNumSamples)
				{
					initializeWithArgs(OSCArgument(minNumSamples));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32		/* MinNumSamples */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/soloBus/addAudioDataListener");
				}

				inline int getMinNumSamples() const
				{
					return jmax(0, getArgumentInt(0));
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloBusAddAudioDataListener)
			};

			class SoloBusRemoveAudioDataListener : public ValidatableOSCMessage
			{
			public:
				SoloBusRemoveAudioDataListener(const OSCMessage& msg) 
					: ValidatableOSCMessage(msg)
				{

				}

				SoloBusRemoveAudioDataListener()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/soloBus/removeAudioDataListener");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloBusRemoveAudioDataListener)
			};
		}
	}
}