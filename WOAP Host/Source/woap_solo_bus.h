#pragma once

#include "woap_mixer_source_node.h"
#include "woap_parameter_float.h"
#include "woap_audio_core.h"
#include "woap_stateful.h"
#include "woap_api_consumer.h"
#include "woap_api_osc_remote_client.h"
#include "woap_sockets_message.h"
#include "woap_audio_data_emitter.h"
#include "woap_bootstrapper.h"

namespace WOAP
{
	class Track;

	class SoloBus : public MixerSourceNode, public AudioDataEmitter, public AudioCore::Listener, public Stateful, public API::APIConsumer
	{
	public:
		class SoloListener : public Parameter::Listener
		{
		public:
			SoloListener(SoloBus* bus, Track* tr) : soloBus(bus), track(tr) {}
			~SoloListener() {}

			virtual void parameterChanged(Parameter*) override;

			SoloBus* soloBus;
			Track* track;
		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloListener)
		};

		enum MessageType
		{
			AddAudioDataListener,
			RemoveAudioDataListener,
		};

		enum Permission
		{
			_BaseId = 1400,
			ListenAudioData,
		};

		SoloBus(AudioCore* core);
		~SoloBus();

		const Array<float>& getSignalLevels() const { return signalLevels; }
		const Array<Track*> getSoloedTracks() const { return soloedTracks; }

		void gatherData(int numSamples);

		virtual void trackAdded(Track*, int position) override;
		virtual void trackRemoved(Track*) override;
		virtual void aboutToDelete(AudioCore*) override {}

		virtual bool canConnectOutputReceiver(const MixerReceiverNode* receiver) const override;
		virtual int getLatencyDiffForNode(const MixerNode* node) const override;
		virtual int getLatencySamplesInTree(bool includeOwnLatency = true) const override;

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;

		virtual Array<API::AddressWithTypeId> getMessageTypesToListenTo() const override;
		virtual void messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId) override;

	protected:
		Array<Track*> soloedTracks;
		OwnedArray<SoloListener> soloListeners;
		Array<float> signalLevels = { 0.0f, 0.0f };

	private:
		const static Bootstrapper::Initializer bootstrapInitializer;
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloBus)
	};
}