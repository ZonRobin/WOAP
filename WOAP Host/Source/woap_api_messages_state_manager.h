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
			class StateManagerSaveState : public ValidatableOSCMessageWithId
			{
			public:
				StateManagerSaveState(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				StateManagerSaveState()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/stateManager/saveState");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateManagerSaveState)
			};

			class StateManagerLoadState : public ValidatableOSCMessageWithId
			{
			public:
				StateManagerLoadState(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				StateManagerLoadState()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/stateManager/loadState");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateManagerLoadState)
			};

			class StateManagerGetState : public ValidatableOSCMessageWithId
			{
			public:
				StateManagerGetState(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				StateManagerGetState()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/stateManager/getState");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateManagerGetState)
			};

			class StateManagerState : public ValidatableOSCMessageWithId
			{
			public:
				StateManagerState(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				StateManagerState(const StateManagerGetState& request, const MemoryOutputStream& os)
				{
					initializeResponseWithArgs(request, OSCArgument(os.toString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* StateString */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/stateManager/state");
				}

				inline String getStateString() const
				{
					return getArgumentString(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateManagerState)
			};

			class StateManagerSetState : public ValidatableOSCMessageWithId
			{
			public:
				StateManagerSetState(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				StateManagerSetState(const MemoryOutputStream& os)
				{
					initializeWithArgs(OSCArgument(os.toString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::blob		/* StateString */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/stateManager/setState");
				}

				inline String getStateString() const
				{
					return getArgumentString(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateManagerSetState)
			};

			class StateManagerProcessResult : public ValidatableOSCMessageWithId
			{
			public:
				StateManagerProcessResult(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				StateManagerProcessResult(const ValidatableOSCMessageWithId& request, bool resultOk)
				{
					initializeResponseWithArgs(request, OSCArgument(boolToInt(resultOk)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32		/* Restult */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/stateManager/processResult");
				}

				inline bool getResult() const
				{
					return getArgumentBool(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateManagerProcessResult)
			};

		}
	}
}