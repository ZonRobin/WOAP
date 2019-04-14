#pragma once

#include "woap_api_listener_brides_osc_bridge_base.h"
#include "woap_audio_core.h"
#include "woap_audio_data_emitter.h"

namespace WOAP
{
	namespace API
	{
		namespace ListenerBridges
		{
			class AudioCoreListenerBridge : public OSCBridgeBase<AudioCore>, public AudioCore::Listener
			{
			public:
				AudioCoreListenerBridge(AudioCore& core, OSC::RemoteClient& client);
				virtual ~AudioCoreListenerBridge();

				virtual void audioInputProxyAdded(AudioInputProxy* newProxy) override;
				virtual void audioOutputProxyAdded(AudioOutputProxy* newProxy) override;
				virtual void audioInputProxyRemoved(AudioInputProxy*) override;
				virtual void audioOutputProxyRemoved(AudioOutputProxy*) override;
				virtual void trackAdded(Track* newTrack, int position) override;
				virtual void trackMoved(Track* track, int oldPosition, int newPosition) override;
				virtual void trackRemoved(Track* oldTrack) override;
				virtual void connectionsChanged() override;
				virtual void globalAudioLatencyChanged(int newLatencySamples, float newLatencyMs) override;
				virtual void aboutToDelete(AudioCore* core) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreListenerBridge)
			};

			class AudioCoreSignalLevelListenerBridge : public OSCBridgeBase<AudioCore>, protected MultiTimer, public AudioCore::SignalLevelListener
			{
			public:
				AudioCoreSignalLevelListenerBridge(AudioCore& core, OSC::RemoteClient& client, int refreshRateHz);
				virtual ~AudioCoreSignalLevelListenerBridge();

				virtual void aboutToDelete(AudioCore* core) override;

				virtual void timerCallback(int timerID) override;
				int fillOutputData();

				/* @return Max value is 24dBFs
				*/
				inline uint16 floatToOutputLevelSample(float value) const
				{
					if (value >= 8.0f)
						return (uint16)65535;
					else if (value <= 0.0f)
						return (uint16)0;
					else
						return (uint16)floorf(value * 8192);
				}

			protected:
				int refreshRate;
				float averaging = 0.5f;
				uint32 maxClipTimeDiffMs;

				Array<float> trackLevels;
				Array<float> soloBusLevels;

				MemoryBlock tracksSignalLevelsData;
				MemoryBlock soloBusSignalLevelsData;
				MemoryBlock tracksClippingMask;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreSignalLevelListenerBridge)
			};

			class AudioCoreAudioDataListenerBridge : public OSCBridgeBase<AudioDataEmitter>, public AudioDataEmitter::Listener
			{
			public:
				AudioCoreAudioDataListenerBridge(AudioDataEmitter& emitter, OSC::RemoteClient& client, int minNumSamples = 512);
				virtual ~AudioCoreAudioDataListenerBridge();

				virtual void audioProcessed(AudioDataEmitter* emitter, const AudioBufferDouble& audioBuffer) override;
				virtual void aboutToDelete(AudioDataEmitter* emitter) override;

			protected:
				int minimalNumSamples;

				MemoryBlock outputChannelData;
				int outputNumSamples = 0;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreAudioDataListenerBridge)
			};
		}
	}
}
