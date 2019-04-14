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
			class AudioCoreAddTrack : public ValidatableOSCMessage
			{
			public:
				AudioCoreAddTrack(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreAddTrack(int trackType, bool addAfterSelectedTrack)
				{
					initializeWithArgs(OSCArgument(trackType), OSCArgument(boolToInt(addAfterSelectedTrack)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* trackType */
						OSCTypes::int32		/* position */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/addTrack");
				}

				inline int getTrackType() const
				{
					return getArgumentInt(0);
				}

				inline int getTrackPosition() const
				{
					return getArgumentInt(1);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreAddTrack)
			};

			class AudioCoreRemoveTrack : public ValidatableOSCMessage
			{
			public:
				AudioCoreRemoveTrack(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreRemoveTrack(const Identifiable* track)
				{
					initializeWithArgs(OSCArgument(track->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string /* trackId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/removeTrack");
				}

				inline Uuid getTrackId() const
				{
					return getArgumentUuid(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreRemoveTrack)
			};

			class AudioCoreMoveTrack : public ValidatableOSCMessage
			{
			public:
				AudioCoreMoveTrack(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreMoveTrack(const Identifiable* track, int newPosition)
				{
					initializeWithArgs(OSCArgument(track->getId().toDashedString()), OSCArgument(newPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string, /* trackId */
						OSCTypes::int32, /* newPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/moveTrack");
				}

				inline Uuid getTrackId() const
				{
					return getArgumentUuid(0);
				}

				inline int getNewPosition() const
				{
					return getArgumentInt(1);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreMoveTrack)
			};

			class AudioCoreTrackMoved : public ValidatableOSCMessage
			{
			public:
				AudioCoreTrackMoved(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreTrackMoved(const Identifiable* track, int oldPosition, int newPosition)
				{
					initializeWithArgs(OSCArgument(track->getId().toDashedString()), OSCArgument(oldPosition), OSCArgument(newPosition));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string, /* trackId */
						OSCTypes::int32, /* oldPosition */
						OSCTypes::int32, /* newPosition */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/trackMoved");
				}

				inline Uuid getTrackId() const
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

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreTrackMoved)
			};

			class AudioCoreConnectNodes : public ValidatableOSCMessage
			{
			public:
				AudioCoreConnectNodes(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreConnectNodes(const Identifiable* source, const Identifiable* receiver)
				{
					initializeWithArgs(OSCArgument(source->getId().toDashedString()), OSCArgument(receiver->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string, /* sourceNodeId */
						OSCTypes::string, /* receiverNodeId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/connectNodes");
				}

				inline Uuid getSourceNodeId() const
				{
					return getArgumentUuid(0);
				}

				inline Uuid getReceiverNodeId() const
				{
					return getArgumentUuid(1);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreConnectNodes)
			};

			class AudioCoreRemoveNodesConnection : public ValidatableOSCMessage
			{
			public:
				AudioCoreRemoveNodesConnection(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreRemoveNodesConnection(const Identifiable* source, const Identifiable* receiver)
				{
					initializeWithArgs(OSCArgument(source->getId().toDashedString()), OSCArgument(receiver->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string, /* sourceNodeId */
						OSCTypes::string, /* receiverNodeId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/removeNodesConnection");
				}

				inline Uuid getSourceNodeId() const
				{
					return getArgumentUuid(0);
				}

				inline Uuid getReceiverNodeId() const
				{
					return getArgumentUuid(1);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreRemoveNodesConnection)
			};

			class AudioCoreGetInputProxiesList : public ValidatableOSCMessageWithId
			{
			public:
				AudioCoreGetInputProxiesList(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreGetInputProxiesList()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/getInputProxiesList");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreGetInputProxiesList)
			};

			class AudioCoreInputProxiesList : public ValidatableOSCMessageWithId
			{
			public:

				AudioCoreInputProxiesList(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreInputProxiesList(const AudioCoreGetInputProxiesList& msg, StringArray proxyIds)
				{
					initializeResponseWithArgs(msg, OSCArgument(JSON::toString(proxyIds)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* proxyIds */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/inputProxiesList");
				}

				inline StringArray getProxyIds() const
				{
					return varToStringArray(JSON::fromString(getArgumentString(0)));
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreInputProxiesList)
			};

			class AudioCoreGetOutputProxiesList : public ValidatableOSCMessageWithId
			{
			public:
				AudioCoreGetOutputProxiesList(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreGetOutputProxiesList()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/getOutputProxiesList");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreGetOutputProxiesList)
			};

			class AudioCoreOutputProxiesList : public ValidatableOSCMessageWithId
			{
			public:

				AudioCoreOutputProxiesList(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreOutputProxiesList(const AudioCoreGetOutputProxiesList& msg, StringArray proxyIds)
				{
					initializeResponseWithArgs(msg, OSCArgument(JSON::toString(proxyIds)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* proxyIds */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/outputProxiesList");
				}

				inline StringArray getProxyIds() const
				{
					return varToStringArray(JSON::fromString(getArgumentString(0)));
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreOutputProxiesList)
			};

			class AudioCoreGetAvailableInputSourcesList : public ValidatableOSCMessageWithId
			{
			public:
				AudioCoreGetAvailableInputSourcesList(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreGetAvailableInputSourcesList(const Identifiable* receiver)
				{
					initializeWithArgs(OSCArgument(receiver->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* receiverNodeId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/getAvailableInputSourcesList");
				}

				inline Uuid getReceiverNodeId() const
				{
					return getArgumentUuid(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreGetAvailableInputSourcesList)
			};

			class AudioCoreAvailableInputSourcesList : public ValidatableOSCMessageWithId
			{
			public:

				AudioCoreAvailableInputSourcesList(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreAvailableInputSourcesList(const AudioCoreGetAvailableInputSourcesList& msg, const Array<MixerSourceNode*>& nodeList, const MixerReceiverNode* receiver)
				{
					var list;
					list.resize(nodeList.size());

					int index = 0;
					for (MixerSourceNode* node : nodeList)
					{
						var nodeData;
						nodeData.append(var(node->getId().toDashedString()));
						nodeData.append(var(node->getName()));
						nodeData.append(var(node->getColour().toDisplayString(false)));
						nodeData.append(var(receiver->hasInputSourceAssigned(node, false)));
						list[index++] = nodeData;
					}

					initializeResponseWithArgs(msg, OSCArgument(JSON::toString(list)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* nodeListJSON */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/availableInputSourcesList");
				}

				inline StringArray getNodeList() const
				{
					return varToStringArray(JSON::fromString(getArgumentString(0)));
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreAvailableInputSourcesList)
			};

			class AudioCoreGetAvailableOutputReceiversList : public ValidatableOSCMessageWithId
			{
			public:
				AudioCoreGetAvailableOutputReceiversList(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreGetAvailableOutputReceiversList(const Identifiable* source)
				{
					initializeWithArgs(OSCArgument(source->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* sourceNodeId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/getAvailableOutputReceiversList");
				}

				inline Uuid getSourceNodeId() const
				{
					return getArgumentUuid(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreGetAvailableOutputReceiversList)
			};

			class AudioCoreAvailableOutputReceiversList : public ValidatableOSCMessageWithId
			{
			public:

				AudioCoreAvailableOutputReceiversList(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreAvailableOutputReceiversList(const AudioCoreGetAvailableOutputReceiversList& msg, const Array<MixerReceiverNode*>& nodeList, const MixerSourceNode* source)
				{
					var list;
					list.resize(nodeList.size());

					int index = 0;
					for (MixerReceiverNode* node : nodeList)
					{
						var nodeData;
						nodeData.append(var(node->getId().toDashedString()));
						nodeData.append(var(node->getName()));
						nodeData.append(var(node->getColour().toDisplayString(false)));
						nodeData.append(var(source->hasOutputReceiverAssigned(node, false)));
						list[index++] = nodeData;
					}

					initializeResponseWithArgs(msg, OSCArgument(JSON::toString(list)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* nodeIds */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/availableOutputReceiversList");
				}

				inline StringArray getReceiverIds() const
				{
					return varToStringArray(JSON::fromString(getArgumentString(0)));
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreAvailableOutputReceiversList)
			};

			class AudioCoreGetTracksList : public ValidatableOSCMessageWithId
			{
			public:
				AudioCoreGetTracksList(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreGetTracksList()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/getTracksList");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreGetTracksList)
			};

			class AudioCoreTracksList : public ValidatableOSCMessageWithId
			{
			public:

				AudioCoreTracksList(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreTracksList(const AudioCoreGetTracksList& msg, StringArray trackIds)
				{
					initializeResponseWithArgs(msg, OSCArgument(JSON::toString(trackIds)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* TrackIds */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/tracksList");
				}

				inline StringArray getTrackIds() const
				{
					return varToStringArray(JSON::fromString(getArgumentString(0)));
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreTracksList)
			};

			class AudioCoreAddListener : public ValidatableOSCMessage
			{
			public:
				AudioCoreAddListener(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreAddListener()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/addListener");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreAddListener)
			};

			class AudioCoreRemoveListener : public ValidatableOSCMessage
			{
			public:
				AudioCoreRemoveListener(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreRemoveListener()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/removeListener");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreRemoveListener)
			};

			class AudioCoreInputProxyAdded : public ValidatableOSCMessage
			{
			public:
				AudioCoreInputProxyAdded(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreInputProxyAdded(const Identifiable* proxy, const String& proxyName)
				{
					initializeWithArgs(OSCArgument(proxy->getId().toDashedString()), OSCArgument(proxyName));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* ProxyId */
						OSCTypes::string	/* ProxyName */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/inputProxyAdded");
				}

				inline Uuid getProxyId() const
				{
					return getArgumentUuid(0);
				}

				inline String getProxyName() const
				{
					return getArgumentString(1);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreInputProxyAdded)
			};

			class AudioCoreInputProxyRemoved : public ValidatableOSCMessage
			{
			public:
				AudioCoreInputProxyRemoved(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreInputProxyRemoved(const Identifiable* proxy)
				{
					initializeWithArgs(OSCArgument(proxy->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* ProxyId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/inputProxyRemoved");
				}

				inline Uuid getProxyId() const
				{
					return getArgumentUuid(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreInputProxyRemoved)
			};

			class AudioCoreOutputProxyAdded : public ValidatableOSCMessage
			{
			public:
				AudioCoreOutputProxyAdded(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreOutputProxyAdded(const Identifiable* proxy, const String& proxyName)
				{
					initializeWithArgs(OSCArgument(proxy->getId().toDashedString()), OSCArgument(proxyName));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* ProxyId */
						OSCTypes::string	/* ProxyName */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/outputProxyAdded");
				}

				inline Uuid getProxyId() const
				{
					return getArgumentUuid(0);
				}

				inline String getProxyName() const
				{
					return getArgumentString(1);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreOutputProxyAdded)
			};

			class AudioCoreOutputProxyRemoved : public ValidatableOSCMessage
			{
			public:
				AudioCoreOutputProxyRemoved(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreOutputProxyRemoved(const Identifiable* proxy)
				{
					initializeWithArgs(OSCArgument(proxy->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* ProxyId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/outputProxyRemoved");
				}

				inline Uuid getProxyId() const
				{
					return getArgumentUuid(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreOutputProxyRemoved)
			};

			class AudioCoreTrackAdded : public ValidatableOSCMessage
			{
			public:
				AudioCoreTrackAdded(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreTrackAdded(const Identifiable* track, int position)
				{
					initializeWithArgs(OSCArgument(track->getId().toDashedString()), OSCArgument(position));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* TrackId */
						OSCTypes::int32		/* Position */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/trackAdded");
				}

				inline Uuid geTracktId() const
				{
					return getArgumentUuid(0);
				}

				inline int getTrackPosition() const
				{
					return getArgumentInt(1);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreTrackAdded)
			};



			class AudioCoreTrackRemoved : public ValidatableOSCMessage
			{
			public:
				AudioCoreTrackRemoved(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreTrackRemoved(const Identifiable* track)
				{
					initializeWithArgs(OSCArgument(track->getId().toDashedString()));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* TrackId */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/trackRemoved");
				}

				inline Uuid getTrackId() const
				{
					return getArgumentUuid(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreTrackRemoved)
			};



			class AudioCoreConnectionsChanged : public ValidatableOSCMessage
			{
			public:
				AudioCoreConnectionsChanged(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreConnectionsChanged()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/connectionsChanged");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreConnectionsChanged)
			};

			class AudioCoreGetGlobalAudioLatency : public ValidatableOSCMessageWithId
			{
			public:
				AudioCoreGetGlobalAudioLatency(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreGetGlobalAudioLatency()
				{
					initializeMessage();
				}


				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/getGlobalAudioLatency");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreGetGlobalAudioLatency)
			};

			class AudioCoreGlobalAudioLatency : public ValidatableOSCMessageWithId
			{
			public:
				AudioCoreGlobalAudioLatency(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				AudioCoreGlobalAudioLatency(const AudioCoreGetGlobalAudioLatency& request, int newLatencySamples, float newLatencyMs)
				{
					initializeResponseWithArgs(request, OSCArgument(newLatencySamples), OSCArgument(newLatencyMs));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* latencySamples */
						OSCTypes::float32	/* latencyMs */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/globalAudioLatency");
				}

				inline int getLatencySamples() const
				{
					return getArgumentInt(0);
				}

				inline float getLatencyMs() const
				{
					return getArgumentFloat(1);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreGlobalAudioLatency)
			};

			class AudioCoreGlobalAudioLatencyChanged : public ValidatableOSCMessage
			{
			public:
				AudioCoreGlobalAudioLatencyChanged(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreGlobalAudioLatencyChanged(int newLatencySamples, float newLatencyMs)
				{
					initializeWithArgs(OSCArgument(newLatencySamples), OSCArgument(newLatencyMs));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* latencySamples */
						OSCTypes::float32	/* latencyMs */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/globalAudioLatencyChanged");
				}

				inline int getLatencySamples() const
				{
					return getArgumentInt(0);
				}

				inline float getLatencyMs() const
				{
					return getArgumentFloat(1);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreGlobalAudioLatencyChanged)
			};

			class AudioCoreAddSignalLevelListener : public ValidatableOSCMessage
			{
			public:
				AudioCoreAddSignalLevelListener(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreAddSignalLevelListener(int refreshRateHz)
				{
					initializeWithArgs(OSCArgument(refreshRateHz));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* refreshRateHz */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/addSignalLevelListener");
				}

				inline int getRefreshRateHz() const
				{
					return getArgumentInt(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreAddSignalLevelListener)
			};

			class AudioCoreRemoveSignalLevelListener : public ValidatableOSCMessage
			{
			public:
				AudioCoreRemoveSignalLevelListener(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreRemoveSignalLevelListener()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/removeSignalLevelListener");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreRemoveSignalLevelListener)
			};

			class AudioCoreSignalLevels : public ValidatableOSCMessage
			{
			public:
				AudioCoreSignalLevels(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreSignalLevels(int numTracks, const MemoryBlock& tracksSignalLevels, const MemoryBlock& soloBusSignalLevels, const MemoryBlock& tracksClippingMask)
				{
					initializeWithArgs(OSCArgument(numTracks), OSCArgument(tracksSignalLevels), OSCArgument(soloBusSignalLevels), OSCArgument(tracksClippingMask));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32, /* number of tracks */
						OSCTypes::blob,	/* tracksSignalLevels int16 data Array<[left, right]> */
						OSCTypes::blob,	/* soloBusSignalLevels int16 data [left, right] */
						OSCTypes::blob	/* bit mask of cliping tracks */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/signalLevels");
				}

				inline int getNumberOfTracks() const
				{
					return getArgumentInt(0);
				}

				inline const MemoryBlock& getTracksSignalLevels() const
				{
					return getArgumentBlob(1);
				}

				inline const MemoryBlock& getSoloBusSignalLevels() const
				{
					return getArgumentBlob(2);
				}

				inline const MemoryBlock& getTracksClippingMask() const
				{
					return getArgumentBlob(3);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreSignalLevels)
			};

			class AudioCoreAudioData : public ValidatableOSCMessage
			{
			public:
				AudioCoreAudioData(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				AudioCoreAudioData(int numSamples, int numChannels, const MemoryBlock& data)
				{					
					initializeWithArgs(OSCArgument(numSamples), OSCArgument(numChannels), OSCArgument(data));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* numSamples */
						OSCTypes::int32,	/* numChannels */
						OSCTypes::blob		/* data (16bit PCM, channels after each other) */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/audioCore/audioData");
				}

				inline int getNumSamples() const
				{
					return getArgumentInt(0);
				}

				inline int getNumChannels() const
				{
					return getArgumentInt(1);
				}

				inline const MemoryBlock& getAudioData() const
				{
					return getArgumentBlob(2);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreAudioData)
			};
		}
	}
}
