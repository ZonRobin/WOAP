#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_mixer_mediator_node.h"
#include "woap_api_consumer.h"
#include "woap_stateful.h"

namespace WOAP
{
	class Track;
	class ParameterFloat;

	class TrackSendNode : public AudioPlaybackInfoListener, public MixerMediatorNode, public API::APIConsumer, public Stateful
	{
	public:
		enum MessageType
		{
			GetDetail,
			SetGain,
			SetPan,
			SetName,
			SetAudioSourcePosition,
			AddInfoListener,
			RemoveInfoListener,
			AddAudioSourcePositionListener,
			RemoveAudioSourcePositionListener,
			AddControlsListener,
			RemoveControlsListener
		};

		class AudioSourcePositionListener
		{
		public:
			virtual void sendAudioSourcePositionChanged(TrackSendNode* send) = 0;
			virtual void aboutToDelete(TrackSendNode* send) = 0;
		};

		TrackSendNode(Track& ownerTrack, int audioSourcePos = -1);
		TrackSendNode(const Uuid& idTouse, Track& ownerTrack, int audioSourcePos = -1);
		virtual ~TrackSendNode();

		virtual void playbackAboutToStart(double newSampleRate, int newBufferSizeSamples) override;
		void processBlock(const AudioBufferDouble& inputBuffer);

		virtual bool canConnectOutputReceiver(const MixerReceiverNode* receiver) const override;
		virtual bool canConnectInputSource(const MixerSourceNode* source) const override;

		virtual bool hasInputSourceAssigned(const MixerSourceNode* sourceNode, bool recursive = true) const;

		ParameterFloat* getGainParameter() const { return gainParameter; }
		ParameterFloat* getPanParameter() const { return panParameter; }
		Track& getOwnerTrack() const { return track; }

		int getAudioSourcePosition() const { return audioSourcePosition; }
		void setAudioSourcePosition(int newPosition);

		void addAudioSourcePositionListener(AudioSourcePositionListener* listener) { audioSourcePositionListeners.add(listener); }
		void removeAudioSourcePositionListener(AudioSourcePositionListener* listener) { audioSourcePositionListeners.remove(listener); }

		virtual int getLatencySamplesInTree(bool includeOwnLatency = true) const override;

		void treeLatencyChanged() { treeLatencySamples = -1; }

		virtual Array<API::AddressWithTypeId> getMessageTypesToListenTo() const;
		virtual void messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId) override;

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;
		static TrackSendNode* fromState(Track& track, const ValueTree& state);

		virtual Colour getDefaultColour();

	protected:
		Track& track;
		int audioSourcePosition = -1;
		
		ScopedPointer<ParameterFloat> gainParameter;
		ScopedPointer<ParameterFloat> panParameter;

		ListenerList<AudioSourcePositionListener> audioSourcePositionListeners;

		OwnedArray<LinearSmoothedValue<double>> channelGains;

		int treeLatencySamples = -1;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSendNode)
	};
}

