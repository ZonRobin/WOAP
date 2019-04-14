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
			class TrackGetDetail : public ValidatableOSCMessageWithId
			{
			public:
				TrackGetDetail(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessageWithId(msg)
				{

				}

				TrackGetDetail(const Identifiable* track)
					: trackId(track->getId().toDashedString())
				{
					initializeMessage();					
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/getDetail");
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackGetDetail)
			};

			class TrackDetail : public ValidatableOSCMessageWithId
			{
			public:
				TrackDetail(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessageWithId(msg)
				{

				}

				TrackDetail(const TrackGetDetail& msg, const Identifiable* track, int trackType, float gainState, float panState, bool muteState, bool soloState, 
					bool recordArmState, const String& name, Colour colour, const StringArray& pluginIds, const StringArray& sendIds)
					: trackId(track->getId().toDashedString())
				{
					initializeResponseWithArgs(
						msg,
						OSCArgument(trackType),
						OSCArgument(gainState),
						OSCArgument(panState),
						OSCArgument(boolToInt(muteState)),
						OSCArgument(boolToInt(soloState)),
						OSCArgument(boolToInt(recordArmState)),
						OSCArgument(name),
						OSCArgument(colour.toDisplayString(false)),
						OSCArgument(JSON::toString(pluginIds)),
						OSCArgument(JSON::toString(sendIds))
					);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/detail");
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* Track type */
						OSCTypes::float32,	/* Gain */
						OSCTypes::float32,	/* Pan */
						OSCTypes::int32,	/* Mute */
						OSCTypes::int32,	/* Solo */
						OSCTypes::int32,	/* Record arm */
						OSCTypes::string,	/* Name */
						OSCTypes::string,	/* Colour */
						OSCTypes::string,	/* PluginIdsJSON */
						OSCTypes::string,	/* SendIdsJSON */
					});
				}

				inline int getTrackType() const
				{
					return getArgumentInt(0);
				}

				inline float getGainState() const
				{
					return getArgumentFloat(1);
				}

				inline float getPanState() const
				{
					return getArgumentFloat(2);
				}

				inline bool getMuteState() const
				{
					return getArgumentBool(3);
				}

				inline bool getSoloState() const
				{
					return getArgumentBool(4);
				}

				inline bool getRecordArmState() const
				{
					return getArgumentBool(5);
				}

				inline String getName() const
				{
					return getArgumentString(6);
				}

				inline String getColour() const
				{
					return getArgumentString(7);
				}

				inline StringArray getPluginIds() const
				{
					return varToStringArray(JSON::fromString(getArgumentString(8)));
				}

				inline StringArray getSendIds() const
				{
					return varToStringArray(JSON::fromString(getArgumentString(9)));
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackDetail)
			};

			class TrackSetGain : public ParameterFloatMessage
			{
			public:
				TrackSetGain(const OSCMessage& msg, const Identifiable* track)
					: ParameterFloatMessage(msg, track)
				{

				}

				TrackSetGain(const Identifiable* track, float newValue)
					: ParameterFloatMessage(track)
				{
					initializeMessage(newValue);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/setGain");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSetGain)
			};

			class TrackSetPan : public ParameterFloatMessage
			{
			public:
				TrackSetPan(const OSCMessage& msg, const Identifiable* track)
					: ParameterFloatMessage(msg, track)
				{

				}

				TrackSetPan(const Identifiable* track, float newValue)
					: ParameterFloatMessage(track)
				{
					initializeMessage(newValue);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/setPan");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSetPan)
			};

			class TrackSetMute : public ParameterBoolMessage
			{
			public:
				TrackSetMute(const OSCMessage& msg, const Identifiable* track)
					: ParameterBoolMessage(msg, track)
				{

				}

				TrackSetMute(const Identifiable* track, bool newState)
					: ParameterBoolMessage(track)
				{
					initializeMessage(newState);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/setMute");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSetMute)
			};

			
			class TrackSetSolo : public ParameterBoolMessage
			{
			public:
				TrackSetSolo(const OSCMessage& msg, const Identifiable* track)
					: ParameterBoolMessage(msg, track)
				{

				}

				TrackSetSolo(const Identifiable* track, bool newState)
					: ParameterBoolMessage(track)
				{
					initializeMessage(newState);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/setSolo");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSetSolo)
			};

			
			class TrackSetRecordArm : public ParameterBoolMessage
			{
			public:
				TrackSetRecordArm(const OSCMessage& msg, const Identifiable* track)
					: ParameterBoolMessage(msg, track)
				{

				}

				TrackSetRecordArm(const Identifiable* track, bool newState)
					: ParameterBoolMessage(track)
				{
					initializeMessage(newState);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/setRecordArm");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSetRecordArm)
			};

			class TrackGainChanged : public ParameterFloatMessage
			{
			public:
				TrackGainChanged(const OSCMessage& msg, const Identifiable* track)
					: ParameterFloatMessage(msg, track)
				{

				}

				TrackGainChanged(const Identifiable* track, float newValue)
					: ParameterFloatMessage(track)
				{
					initializeMessage(newValue);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/gainChanged");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackGainChanged)
			};

			class TrackPanChanged : public ParameterFloatMessage
			{
			public:
				TrackPanChanged(const OSCMessage& msg, const Identifiable* track)
					: ParameterFloatMessage(msg, track)
				{

				}

				TrackPanChanged(const Identifiable* track, float newValue)
					: ParameterFloatMessage(track)
				{
					initializeMessage(newValue);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/panChanged");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPanChanged)
			};

			class TrackMuteChanged : public ParameterBoolMessage
			{
			public:
				TrackMuteChanged(const OSCMessage& msg, const Identifiable* track)
					: ParameterBoolMessage(msg, track)
				{

				}

				TrackMuteChanged(const Identifiable* track, bool newState)
					: ParameterBoolMessage(track)
				{
					initializeMessage(newState);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/muteChanged");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackMuteChanged)
			};

			class TrackSoloChanged : public ParameterBoolMessage
			{
			public:
				TrackSoloChanged(const OSCMessage& msg, const Identifiable* track)
					: ParameterBoolMessage(msg, track)
				{

				}

				TrackSoloChanged(const Identifiable* track, bool newState)
					: ParameterBoolMessage(track)
				{
					initializeMessage(newState);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/soloChanged");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSoloChanged)
			};

			class TrackRecordArmChanged : public ParameterBoolMessage
			{
			public:
				TrackRecordArmChanged(const OSCMessage& msg, const Identifiable* track)
					: ParameterBoolMessage(msg, track)
				{

				}

				TrackRecordArmChanged(const Identifiable* track, bool newState)
					: ParameterBoolMessage(track)
				{
					initializeMessage(newState);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/recordArmChanged");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackRecordArmChanged)
			};


			class TrackAddInfoListener : public ValidatableOSCMessage
			{
			public:
				TrackAddInfoListener(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackAddInfoListener(const Identifiable* track)
					: trackId(track->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/addInfoListener");
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackAddInfoListener)
			};

			class TrackRemoveInfoListener : public ValidatableOSCMessage
			{
			public:
				TrackRemoveInfoListener(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackRemoveInfoListener(const Identifiable* track)
					: trackId(track->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/removeInfoListener");
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackRemoveInfoListener)
			};

			class TrackAddControlsListener : public ValidatableOSCMessage
			{
			public:
				TrackAddControlsListener(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackAddControlsListener(const Identifiable* track)
					: trackId(track->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/addControlsListener");
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackAddControlsListener)
			};

			class TrackRemoveControlsListener : public ValidatableOSCMessage
			{
			public:
				TrackRemoveControlsListener(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackRemoveControlsListener(const Identifiable* track)
					: trackId(track->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/removeControlsListener");
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackRemoveControlsListener)
			};

			class TrackAddPluginsListener : public ValidatableOSCMessage
			{
			public:
				TrackAddPluginsListener(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackAddPluginsListener(const Identifiable* track)
					: trackId(track->getId().toDashedString())
				{
					initializeMessage();					
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/addPluginsListener");
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackAddPluginsListener)
			};

			class TrackRemovePluginsListener : public ValidatableOSCMessage
			{
			public:
				TrackRemovePluginsListener(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackRemovePluginsListener(const Identifiable* track)
					: trackId(track->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/removePluginsListener");
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackRemovePluginsListener)
			};

			class TrackAddSendsListener : public ValidatableOSCMessage
			{
			public:
				TrackAddSendsListener(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackAddSendsListener(const Identifiable* track)
					: trackId(track->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/addSendsListener");
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackAddSendsListener)
			};

			class TrackRemoveSendsListener : public ValidatableOSCMessage
			{
			public:
				TrackRemoveSendsListener(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackRemoveSendsListener(const Identifiable* track)
					: trackId(track->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/removeSendsListener");
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackRemoveSendsListener)
			};

			class TrackSetName : public ValidatableOSCMessage
			{
			public:
				TrackSetName(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackSetName(const Identifiable* track, const String& newName)
					: trackId(track->getId().toDashedString())
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
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/setName");
				}

				inline String getNewName() const
				{
					return getArgumentString(0);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSetName)
			};

			class TrackNameChanged : public ValidatableOSCMessage
			{
			public:
				TrackNameChanged(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackNameChanged(const Identifiable* track, const String& newName)
					: trackId(track->getId().toDashedString())
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
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/nameChanged");
				}

				inline String getNewName() const
				{
					return getArgumentString(0);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackNameChanged)
			};


			class TrackSetColour : public ValidatableOSCMessage
			{
			public:
				TrackSetColour(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackSetColour(const Identifiable* track, Colour newColour)
					: trackId(track->getId().toDashedString())
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
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/setColour");
				}

				inline Colour getNewColour() const
				{
					return Colour::fromString("FF" + getArgumentString(0));
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSetColour)
			};

			class TrackColourChanged : public ValidatableOSCMessage
			{
			public:
				TrackColourChanged(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackColourChanged(const Identifiable* track, Colour newColour)
					: trackId(track->getId().toDashedString())
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
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/colourChanged");
				}

				inline Colour getNewColour() const
				{
					return Colour::fromString(getArgumentString(0));
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackColourChanged)
			};


			class TrackAddSendNode : public ValidatableOSCMessage
			{
			public:
				TrackAddSendNode(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackAddSendNode(const Identifiable* track, int audioSourcePosition, int sendPosition)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(audioSourcePosition), OSCArgument(sendPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* audioSourcePosition */
						OSCTypes::int32		/* sendPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/addSendNode");
				}

				inline int getAudioSourcePosition() const
				{
					return getArgumentInt(0);
				}

				inline int getSendPosition() const
				{
					return getArgumentInt(1);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackAddSendNode)
			};

			class TrackRemoveSendNode : public ValidatableOSCMessage
			{
			public:
				TrackRemoveSendNode(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackRemoveSendNode(const Identifiable* track, const Identifiable* send)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(send->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* sendId */
						});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/removeSendNode");
				}

				inline Uuid getSendId() const
				{
					return getArgumentUuid(0);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackRemoveSendNode)
			};

			class TrackMoveSendNode : public ValidatableOSCMessage
			{
			public:
				TrackMoveSendNode(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackMoveSendNode(const Identifiable* track, const Identifiable* send, int newPosition)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(send->getId().toDashedString()), OSCArgument(newPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* sendId */
						OSCTypes::int32		/* newPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/moveSendNode");
				}

				inline Uuid getSendId() const
				{
					return getArgumentUuid(0);
				}

				inline int getNewPosition() const
				{
					return getArgumentInt(1);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackMoveSendNode)
			};

			class TrackAddPlugin : public ValidatableOSCMessage
			{
			public:
				TrackAddPlugin(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackAddPlugin(const Identifiable* track, int pluginTypeId, int pluginPosition)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(pluginTypeId), OSCArgument(pluginPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* pluginTypeId */
						OSCTypes::int32		/* pluginPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/addPlugin");
				}

				inline int getPluginTypeId() const
				{
					return getArgumentInt(0);
				}

				inline int getPluginPosition() const
				{
					return getArgumentInt(1);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackAddPlugin)
			};

			class TrackPluginAdded : public ValidatableOSCMessage
			{
			public:
				TrackPluginAdded(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackPluginAdded(const Identifiable* track, const Identifiable* plugin, int position)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(plugin->getId().toDashedString()), OSCArgument(position));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* PluginId */
						OSCTypes::int32,	/* PluginPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/pluginAdded");
				}

				inline Uuid getPluginId() const
				{
					return getArgumentUuid(0);
				}

				inline int getPluginPosition() const
				{
					return getArgumentInt(1);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPluginAdded)
			};

			class TrackRemovePlugin : public ValidatableOSCMessage
			{
			public:
				TrackRemovePlugin(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackRemovePlugin(const Identifiable* track, const Identifiable* plugin)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(plugin->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* PluginId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/removePlugin");
				}

				inline Uuid getPluginId() const
				{
					return getArgumentUuid(0);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackRemovePlugin)
			};

			class TrackPluginRemoved : public ValidatableOSCMessage
			{
			public:
				TrackPluginRemoved(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackPluginRemoved(const Identifiable* track, const Identifiable* plugin)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(plugin->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* PluginId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/pluginRemoved");
				}

				inline Uuid getPluginId() const
				{
					return getArgumentUuid(0);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPluginRemoved)
			};

			class TrackMovePlugin : public ValidatableOSCMessage
			{
			public:
				TrackMovePlugin(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackMovePlugin(const Identifiable* track, const Identifiable* plugin, int newPosition)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(plugin->getId().toDashedString()), OSCArgument(newPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* PluginId */,
						OSCTypes::int32		/* newPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/movePlugin");
				}

				inline Uuid getPluginId() const
				{
					return getArgumentUuid(0);
				}

				inline int getNewPosition() const
				{
					return getArgumentInt(1);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackMovePlugin)
			};

			class TrackPluginMoved : public ValidatableOSCMessage
			{
			public:
				TrackPluginMoved(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackPluginMoved(const Identifiable* track, const Identifiable* plugin, int oldPosition, int newPosition)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(plugin->getId().toDashedString()), OSCArgument(oldPosition), OSCArgument(newPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* PluginId */
						OSCTypes::int32,	/* OldPosition */
						OSCTypes::int32,	/* NewPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/pluginMoved");
				}

				inline Uuid getPluginId() const
				{
					return getArgumentUuid(0);
				}

				inline int getOldPosition() const
				{
					return getArgumentInt(1);
				}

				inline int getNewPosition() const
				{
					return getArgumentInt(2);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPluginMoved)
			};


			class TrackSendAdded : public ValidatableOSCMessage
			{
			public:
				TrackSendAdded(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackSendAdded(const Identifiable* track, const Identifiable* send, int sendPosition)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(send->getId().toDashedString()), OSCArgument(sendPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* SendId */
						OSCTypes::int32		/* SendPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/sendAdded");
				}

				inline Uuid getSendId() const
				{
					return getArgumentUuid(0);
				}

				inline int getSendPosition() const
				{
					return getArgumentInt(1);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSendAdded)
			};


			class TrackSendRemoved : public ValidatableOSCMessage
			{
			public:
				TrackSendRemoved(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackSendRemoved(const Identifiable* track, const Identifiable* send)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(send->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* SendId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/sendRemoved");
				}

				inline Uuid getSendId() const
				{
					return getArgumentUuid(0);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSendRemoved)
			};

			class TrackSendMoved : public ValidatableOSCMessage
			{
			public:
				TrackSendMoved(const OSCMessage& msg, const Identifiable* track)
					: trackId(track->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				TrackSendMoved(const Identifiable* track, const Identifiable* send, int oldPosition, int newPosition)
					: trackId(track->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(send->getId().toDashedString()), OSCArgument(oldPosition), OSCArgument(newPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* SendId */
						OSCTypes::int32,	/* OldPosition */
						OSCTypes::int32,	/* NewPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(trackId);
				}

				inline static String getAddress(const String& trackId)
				{
					return String("/track/" + trackId + "/sendMoved");
				}

				inline Uuid getSendId() const
				{
					return getArgumentUuid(0);
				}

				inline int getOldPosition() const
				{
					return getArgumentInt(1);
				}

				inline int getNewPosition() const
				{
					return getArgumentInt(2);
				}

			protected:
				String trackId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSendMoved)
			};
		}
	}
}
