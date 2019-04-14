#pragma once

#include "woap_api_listener_brides_osc_bridge_base.h"
#include "woap_track.h"
#include "woap_parameter.h"

namespace WOAP
{
	class AudioProcessorPlayerDouble;

	namespace API
	{
		namespace ListenerBridges
		{
			class TrackInfoListenerBridge : public OSCBridgeBase<Track>, public Track::Listener
			{
			public:
				TrackInfoListenerBridge(Track& track, OSC::RemoteClient& client);
				virtual ~TrackInfoListenerBridge();

				virtual void nameChanged(MixerNode* node) override;
				virtual void colourChanged(MixerNode* node) override;
				virtual void aboutToDelete(MixerNode* node) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackInfoListenerBridge)
			};

			class TrackGainListenerBridge : public OSCBridgeBase<Track>, public WOAP::Parameter::Listener
			{
			public:
				TrackGainListenerBridge(Track& track, OSC::RemoteClient& client);
				virtual ~TrackGainListenerBridge();

				virtual void parameterChanged(Parameter*) override;
				virtual void parameterWillBeDeleted(Parameter*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackGainListenerBridge)
			};

			class TrackPanListenerBridge : public OSCBridgeBase<Track>, public WOAP::Parameter::Listener
			{
			public:
				TrackPanListenerBridge(Track& track, OSC::RemoteClient& client);
				virtual ~TrackPanListenerBridge();

				virtual void parameterChanged(Parameter*) override;
				virtual void parameterWillBeDeleted(Parameter*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPanListenerBridge)
			};

			class TrackMuteListenerBridge : public OSCBridgeBase<Track>, public WOAP::Parameter::Listener
			{
			public:
				TrackMuteListenerBridge(Track& track, OSC::RemoteClient& client);
				virtual ~TrackMuteListenerBridge();

				virtual void parameterChanged(Parameter*) override;
				virtual void parameterWillBeDeleted(Parameter*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackMuteListenerBridge)
			};

			class TrackSoloListenerBridge : public OSCBridgeBase<Track>, public WOAP::Parameter::Listener
			{
			public:
				TrackSoloListenerBridge(Track& track, OSC::RemoteClient& client);
				virtual ~TrackSoloListenerBridge();

				virtual void parameterChanged(Parameter*) override;
				virtual void parameterWillBeDeleted(Parameter*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSoloListenerBridge)
			};

			class TrackRecordArmListenerBridge : public OSCBridgeBase<Track>, public WOAP::Parameter::Listener
			{
			public:
				TrackRecordArmListenerBridge(Track& track, OSC::RemoteClient& client);
				virtual ~TrackRecordArmListenerBridge();

				virtual void parameterChanged(Parameter*) override;
				virtual void parameterWillBeDeleted(Parameter*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackRecordArmListenerBridge)
			};

			class TrackPluginsListenerBridge : public OSCBridgeBase<Track>, public Track::PluginsListener
			{
			public:
				TrackPluginsListenerBridge(Track& track, OSC::RemoteClient& client);
				virtual ~TrackPluginsListenerBridge();

				virtual void pluginAdded(AudioProcessorPlayerDouble* plugin, int position) override;
				virtual void pluginRemoved(AudioProcessorPlayerDouble* plugin) override;
				virtual void pluginMoved(AudioProcessorPlayerDouble* plugin, int oldPosition, int newPosition) override;
				virtual void aboutToDelete(Track*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPluginsListenerBridge)
			};

			class TrackSendsListenerBridge : public OSCBridgeBase<Track>, public Track::SendsListener
			{
			public:
				TrackSendsListenerBridge(Track& track, OSC::RemoteClient& client);
				virtual ~TrackSendsListenerBridge();

				virtual void sendAdded(TrackSendNode* node, int position) override;
				virtual void sendRemoved(TrackSendNode* node) override;
				virtual void sendMoved(TrackSendNode* node, int oldPosition, int newPosition) override;

				virtual void aboutToDelete(Track*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSendsListenerBridge)
			};
		}
	}
}
