#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_api_messages_validatable_osc_message.h"
#include "woap_api_messages_validatable_osc_message_with_id.h"
#include "woap_api_messages_parameter_message.h"

namespace WOAP
{
	namespace API
	{
		namespace Messages
		{
			class SendGetDetail : public ValidatableOSCMessageWithId
			{
			public:
				SendGetDetail(const OSCMessage& msg, const Identifiable* send)
					: sendNodeId(send->getId().toDashedString()), ValidatableOSCMessageWithId(msg)
				{

				}

				SendGetDetail(const Identifiable* send)
					: sendNodeId(send->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendNodeId);
				}

				inline static String getAddress(const String& sendNodeId)
				{
					return String("/send/" + sendNodeId + "/getDetail");
				}

			protected:
				String sendNodeId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendGetDetail)
			};

			class SendDetail : public ValidatableOSCMessageWithId
			{
			public:
				SendDetail(const OSCMessage& msg, const Identifiable* send)
					: sendNodeId(send->getId().toDashedString()), ValidatableOSCMessageWithId(msg)
				{

				}

				SendDetail(const SendGetDetail& msg, const Identifiable* send, float gainState, float panState, const String& name, Colour colour, int audioSourcePosition)
					: sendNodeId(send->getId().toDashedString())
				{
					initializeResponseWithArgs(
						msg,
						OSCArgument(gainState),
						OSCArgument(panState),
						OSCArgument(name),
						OSCArgument(colour.toDisplayString(false)),
						OSCArgument(audioSourcePosition)
					);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendNodeId);
				}

				inline static String getAddress(const String& sendNodeId)
				{
					return String("/send/" + sendNodeId + "/detail");
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::float32,	/* Gain */
						OSCTypes::float32,	/* Pan */
						OSCTypes::string,	/* Name */
						OSCTypes::string,	/* Colour */
						OSCTypes::int32		/* Audio source position */
					});
				}

				inline float getGainState() const
				{
					return getArgumentFloat(0);
				}

				inline float getPanState() const
				{
					return getArgumentFloat(1);
				}

				inline String getName() const
				{
					return getArgumentString(2);
				}

				inline String getColour() const
				{
					return getArgumentString(3);
				}

				inline int getAudioSourcePosition() const
				{
					return getArgumentInt(4);
				}

			protected:
				String sendNodeId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendDetail)
			};

			class SendSetGain : public ParameterFloatMessage
			{
			public:
				SendSetGain(const OSCMessage& msg, const Identifiable* send)
					: ParameterFloatMessage(msg, send)
				{

				}

				SendSetGain(const Identifiable* send, float newValue)
					: ParameterFloatMessage(send)
				{
					initializeMessage(newValue);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/setGain");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendSetGain)
			};

			class SendGainChanged : public ParameterFloatMessage
			{
			public:
				SendGainChanged(const OSCMessage& msg, const Identifiable* send)
					: ParameterFloatMessage(msg, send)
				{

				}

				SendGainChanged(const Identifiable* send, float newValue)
					: ParameterFloatMessage(send)
				{
					initializeMessage(newValue);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/gainChanged");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendGainChanged)
			};

			class SendSetPan : public ParameterFloatMessage
			{
			public:
				SendSetPan(const OSCMessage& msg, const Identifiable* send)
					: ParameterFloatMessage(msg, send)
				{

				}

				SendSetPan(const Identifiable* send, float newValue)
					: ParameterFloatMessage(send)
				{
					initializeMessage(newValue);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/setPan");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendSetPan)
			};

			class SendPanChanged : public ParameterFloatMessage
			{
			public:
				SendPanChanged(const OSCMessage& msg, const Identifiable* send)
					: ParameterFloatMessage(msg, send)
				{

				}

				SendPanChanged(const Identifiable* send, float newValue)
					: ParameterFloatMessage(send)
				{
					initializeMessage(newValue);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/panChanged");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendPanChanged)
			};

			class SendSetAudioSourcePosition : public ValidatableOSCMessage
			{
			public:
				SendSetAudioSourcePosition(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendSetAudioSourcePosition(const Identifiable* send, int newPosition)
					: sendId(send->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(newPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32	/* newPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/setAudioSourcePosition");
				}

				inline int getNewPosition() const
				{
					return getArgumentInt(0);
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendSetAudioSourcePosition)
			};

			class SendAudioSourcePositionChanged : public ValidatableOSCMessage
			{
			public:
				SendAudioSourcePositionChanged(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendAudioSourcePositionChanged(const Identifiable* send, int newPosition)
					: sendId(send->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(newPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32	/* newPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/audioSourcePositionChanged");
				}

				inline int getNewPosition() const
				{
					return getArgumentInt(0);
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendAudioSourcePositionChanged)
			};

			class SendSetName : public ValidatableOSCMessage
			{
			public:
				SendSetName(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendSetName(const Identifiable* send, const String& newName)
					: sendId(send->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(newName));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* NewName */
						});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/setName");
				}

				inline String getNewName() const
				{
					return getArgumentString(0);
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendSetName)
			};

			class SendNameChanged : public ValidatableOSCMessage
			{
			public:
				SendNameChanged(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendNameChanged(const Identifiable* send, const String& newName)
					: sendId(send->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(newName));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* NewName */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/nameChanged");
				}

				inline String getNewName() const
				{
					return getArgumentString(0);
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendNameChanged)
			};

			class SendColourChanged : public ValidatableOSCMessage
			{
			public:
				SendColourChanged(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendColourChanged(const Identifiable* send, Colour newColour)
					: sendId(send->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(newColour.toDisplayString(false)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* NewColour */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/colourChanged");
				}

				inline Colour getNewColour() const
				{
					return Colour::fromString(getArgumentString(0));
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendColourChanged)
			};


			class SendAddAudioSourcePositionListener : public ValidatableOSCMessage
			{
			public:
				SendAddAudioSourcePositionListener(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendAddAudioSourcePositionListener(const Identifiable* send)
					: sendId(send->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/addAudioSourcePositionListener");
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendAddAudioSourcePositionListener)
			};

			class SendRemoveAudioSourcePositionListener : public ValidatableOSCMessage
			{
			public:
				SendRemoveAudioSourcePositionListener(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendRemoveAudioSourcePositionListener(const Identifiable* send)
					: sendId(send->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/removeAudioSourcePositionListener");
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendRemoveAudioSourcePositionListener)
			};

			class SendAddInfoListener : public ValidatableOSCMessage
			{
			public:
				SendAddInfoListener(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendAddInfoListener(const Identifiable* send)
					: sendId(send->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/addInfoListener");
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendAddInfoListener)
			};

			class SendRemoveInfoListener : public ValidatableOSCMessage
			{
			public:
				SendRemoveInfoListener(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendRemoveInfoListener(const Identifiable* send)
					: sendId(send->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/removeInfoListener");
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendRemoveInfoListener)
			};

			class SendAddControlsListener : public ValidatableOSCMessage
			{
			public:
				SendAddControlsListener(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendAddControlsListener(const Identifiable* send)
					: sendId(send->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/addControlsListener");
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendAddControlsListener)
			};

			class SendRemoveControlsListener : public ValidatableOSCMessage
			{
			public:
				SendRemoveControlsListener(const OSCMessage& msg, const Identifiable* send)
					: sendId(send->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				SendRemoveControlsListener(const Identifiable* send)
					: sendId(send->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(sendId);
				}

				inline static String getAddress(const String& sendId)
				{
					return String("/send/" + sendId + "/removeControlsListener");
				}

			protected:
				String sendId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendRemoveControlsListener)
			};
		}
	}
}