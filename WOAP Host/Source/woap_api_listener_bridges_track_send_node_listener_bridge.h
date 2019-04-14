#pragma once

#include "woap_api_listener_brides_osc_bridge_base.h"
#include "woap_track_send_node.h"
#include "woap_parameter.h"

namespace WOAP
{
	namespace API
	{
		namespace ListenerBridges
		{
			class SendInfoListenerBridge : public OSCBridgeBase<TrackSendNode>, public TrackSendNode::Listener
			{
			public:
				SendInfoListenerBridge(TrackSendNode& send, OSC::RemoteClient& client);
				virtual ~SendInfoListenerBridge();

				virtual void nameChanged(MixerNode* node) override;
				virtual void colourChanged(MixerNode*) override;
				virtual void aboutToDelete(MixerNode* node) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendInfoListenerBridge)
			};

			class SendGainListenerBridge : public OSCBridgeBase<TrackSendNode>, public WOAP::Parameter::Listener
			{
			public:
				SendGainListenerBridge(TrackSendNode& send, OSC::RemoteClient& client);
				virtual ~SendGainListenerBridge();

				virtual void parameterChanged(Parameter*) override;
				virtual void parameterWillBeDeleted(Parameter*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendGainListenerBridge)
			};

			class SendPanListenerBridge : public OSCBridgeBase<TrackSendNode>, public WOAP::Parameter::Listener
			{
			public:
				SendPanListenerBridge(TrackSendNode& send, OSC::RemoteClient& client);
				virtual ~SendPanListenerBridge();

				virtual void parameterChanged(Parameter*) override;
				virtual void parameterWillBeDeleted(Parameter*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendPanListenerBridge)
			};

			class SendAudioSourcePositionListenerBridge : public OSCBridgeBase<TrackSendNode>, public TrackSendNode::AudioSourcePositionListener
			{
			public:
				SendAudioSourcePositionListenerBridge(TrackSendNode& send, OSC::RemoteClient& client);
				virtual ~SendAudioSourcePositionListenerBridge();

				virtual void sendAudioSourcePositionChanged(TrackSendNode* send) override;
				virtual void aboutToDelete(TrackSendNode* send) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SendAudioSourcePositionListenerBridge)
			};
		}
	}
}
