#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_api_messages_validatable_osc_message.h"
#include "woap_api_messages_validatable_osc_message_with_id.h"
#include "woap_functions.h"

namespace WOAP
{
	namespace API
	{
		namespace Messages
		{
			class ApiCoreHeartbeat : public ValidatableOSCMessage
			{
			public:
				ApiCoreHeartbeat(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				ApiCoreHeartbeat()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/heartbeat");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreHeartbeat)
			};

			class ApiCoreGetStatus : public ValidatableOSCMessage
			{
			public:
				ApiCoreGetStatus(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				ApiCoreGetStatus()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/getStatus");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreGetStatus)
			};

			class ApiCoreReady : public ValidatableOSCMessage
			{
			public:
				ApiCoreReady(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				ApiCoreReady()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/ready");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreReady)
			};

			class ApiCoreError : public ValidatableOSCMessage
			{
			public:
				ApiCoreError(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				ApiCoreError()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/error");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreError)
			};

			class ApiCoreMessageRejected : public ValidatableOSCMessage
			{
			public:
				enum Reason
				{
					InvalidMessage = 1001,
					Unauthorized,
					ParsingError
				};

				ApiCoreMessageRejected(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				ApiCoreMessageRejected(const Messages::ValidatableOSCMessage& rejectedMsg, Reason reason)
				{
					int rejectedMessageId = -1;
					if (const Messages::ValidatableOSCMessageWithId* m = dynamic_cast<const Messages::ValidatableOSCMessageWithId*>(&rejectedMsg))
					{
						rejectedMessageId = m->getMessageId();
					}

					initializeWithArgs(
						OSCArgument(reason), 
						OSCArgument(rejectedMessageId), 
						OSCArgument(rejectedMsg.getAddress() + " with " + String(rejectedMsg.getNumArguments()) + " arguments")
					);
				}

				ApiCoreMessageRejected(const String& errorMessage, Reason reason)
				{
					initializeWithArgs(OSCArgument(reason), OSCArgument(-1), OSCArgument(errorMessage));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* Reason */
						OSCTypes::int32,	/* Rejected message id */
						OSCTypes::string	/* Debug info */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/messageRejected");
				}

				inline int32 getReason() const
				{
					return getArgumentInt(0);
				}

				inline int32 getRejectedMessageId() const
				{
					return getArgumentInt(1);
				}

				inline String getDebugInfo() const
				{
					return getArgumentString(2);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreMessageRejected)
			};

			class ApiCoreGetServerInfo : public ValidatableOSCMessageWithId
			{
			public:
				ApiCoreGetServerInfo(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				ApiCoreGetServerInfo()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/getServerInfo");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreGetServerInfo)
			};

			class ApiCoreServerInfo : public ValidatableOSCMessageWithId
			{
			public:
				ApiCoreServerInfo(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				ApiCoreServerInfo(const ApiCoreGetServerInfo& msg, const String& deviceVersion, const Uuid& deviceId, const Uuid& processId, bool anonymousLoginEnabled, bool userAuthenticated)
				{
					initializeResponseWithArgs(
						msg,
						OSCArgument(deviceVersion),
						OSCArgument(deviceId.toDashedString()),
						OSCArgument(processId.toDashedString()),
						OSCArgument(boolToInt(anonymousLoginEnabled)),
						OSCArgument(boolToInt(userAuthenticated))
					);
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* deviceVersion */
						OSCTypes::string,	/* deviceId */
						OSCTypes::string,	/* processId */
						OSCTypes::int32,	/* anonymousLoginEnabled */
						OSCTypes::int32		/* userAuthenticated */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/serverInfo");
				}

				inline String getDeviceVersion() const
				{
					return getArgumentString(0);
				}

				inline String getDeviceId() const
				{
					return getArgumentString(1);
				}

				inline String getProcessId() const
				{
					return getArgumentString(2);
				}
				
				inline bool getAnonymousLoginEnabled() const
				{
					return getArgumentBool(3);
				}

				inline bool getUserAuthenticated() const
				{
					return getArgumentBool(4);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreServerInfo)
			};

			class ApiCoreLogin : public ValidatableOSCMessageWithId
			{
			public:
				ApiCoreLogin(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				ApiCoreLogin(const String& password)
				{
					initializeWithArgs(OSCArgument(password));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* Password */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/login");
				}

				inline String getPassword() const
				{
					return getArgumentString(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreLogin)
			};

			class ApiCoreLoginResponse : public ValidatableOSCMessageWithId
			{
			public:
				enum ResponseCode
				{
					Ok = 1000,
					WrongCredentials = 1001
				};

				ApiCoreLoginResponse(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				ApiCoreLoginResponse(const ApiCoreLogin& msg, ResponseCode responseCode, const String& logTag = "")
				{
					initializeResponseWithArgs(msg, OSCArgument(responseCode), OSCArgument(logTag));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* Response code */
						OSCTypes::string	/* Log tag */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/loginResponse");
				}

				inline int32 getResponseCode() const
				{
					return getArgumentInt(0);
				}

				inline String getLogTag() const
				{
					return getArgumentString(1);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreLoginResponse)
			};

			class ApiCoreAuthenticate : public ValidatableOSCMessageWithId
			{
			public:
				ApiCoreAuthenticate(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				ApiCoreAuthenticate(const String& logTag)
				{
					initializeWithArgs(OSCArgument(logTag));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* Log tag */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/authenticate");
				}

				inline String getLogTag() const
				{
					return getArgumentString(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreAuthenticate)
			};

			class ApiCoreAuthenticationResponse : public ValidatableOSCMessageWithId
			{
			public:
				enum ResponseCode
				{
					Ok = 1000,
					LogTagUnknownOrExpired = 1001
				};

				ApiCoreAuthenticationResponse(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				ApiCoreAuthenticationResponse(const ApiCoreAuthenticate& msg, ResponseCode responseCode)
				{
					initializeResponseWithArgs(msg, OSCArgument(responseCode));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32		/* Response code */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/authenticationResponse");
				}

				inline int32 getResponseCode() const
				{
					return getArgumentInt(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ApiCoreAuthenticationResponse)
			};
		}
	}
}
