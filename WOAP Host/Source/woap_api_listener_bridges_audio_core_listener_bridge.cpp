#include "woap_api_listener_bridges_audio_core_listener_bridge.h"
#include "woap_api_osc_remote_client.h"
#include "woap_audio_input_proxy.h"
#include "woap_audio_output_proxy.h"
#include "woap_track.h"
#include "woap_api_messages_audio_core.h"
#include "woap_solo_bus.h"

using namespace WOAP;
using namespace WOAP::API::ListenerBridges;
using namespace WOAP::API::OSC;

AudioCoreListenerBridge::AudioCoreListenerBridge(AudioCore& core, RemoteClient& client)
	: OSCBridgeBase(core, client)
{

}

AudioCoreListenerBridge::~AudioCoreListenerBridge()
{
	trackedInstance.removeListener(this);
}

void AudioCoreListenerBridge::audioInputProxyAdded(WOAP::AudioInputProxy* newProxy)
{
	client.sendOSCMessage(Messages::AudioCoreInputProxyAdded(newProxy, newProxy->getName()));
}

void AudioCoreListenerBridge::audioOutputProxyAdded(WOAP::AudioOutputProxy* newProxy)
{
	client.sendOSCMessage(Messages::AudioCoreOutputProxyAdded(newProxy, newProxy->getName()));
}

void AudioCoreListenerBridge::audioInputProxyRemoved(AudioInputProxy* proxy)
{
	client.sendOSCMessage(Messages::AudioCoreInputProxyRemoved(proxy));
}

void AudioCoreListenerBridge::audioOutputProxyRemoved(AudioOutputProxy* proxy)
{
	client.sendOSCMessage(Messages::AudioCoreOutputProxyRemoved(proxy));
}

void AudioCoreListenerBridge::trackAdded(WOAP::Track* newTrack, int position)
{
	client.sendOSCMessage(Messages::AudioCoreTrackAdded(newTrack, position));
}

void AudioCoreListenerBridge::trackMoved(Track* track, int oldPosition, int newPosition)
{
	client.sendOSCMessage(Messages::AudioCoreTrackMoved(track, oldPosition, newPosition));
}

void AudioCoreListenerBridge::trackRemoved(WOAP::Track* oldTrack)
{
	client.sendOSCMessage(Messages::AudioCoreTrackRemoved(oldTrack));
}

void AudioCoreListenerBridge::connectionsChanged()
{
	client.sendOSCMessage(Messages::AudioCoreConnectionsChanged());
}

void AudioCoreListenerBridge::globalAudioLatencyChanged(int newLatencySamples, float newLatencyMs)
{
	client.sendOSCMessage(Messages::AudioCoreGlobalAudioLatencyChanged(newLatencySamples, newLatencyMs));
}

void AudioCoreListenerBridge::aboutToDelete(AudioCore*)
{
	client.removeListenerBridge(this);
}





AudioCoreSignalLevelListenerBridge::AudioCoreSignalLevelListenerBridge(AudioCore& core, RemoteClient& client, int refreshRateHz)
	: refreshRate(jlimit(1, 30, refreshRateHz)), OSCBridgeBase(core, client)
{
	maxClipTimeDiffMs = (uint32)2000 / refreshRate;

	tracksSignalLevelsData.setSize(sizeof(uint16) * 2 * trackedInstance.getTracks().size());
	soloBusSignalLevelsData.setSize(sizeof(uint16) * 2);

	trackLevels.resize(2 * trackedInstance.getTracks().size());
	soloBusLevels.resize(2);

	startTimer(0, (int)(1000.0f / jmin(100, refreshRate * 5)));
	startTimer(1, (int)(1000.0f / refreshRate));

	averaging = (float)refreshRate / jmin(100, refreshRate * 5);
}

AudioCoreSignalLevelListenerBridge::~AudioCoreSignalLevelListenerBridge()
{
	trackedInstance.removeSignalLevelListener(this);
}

void AudioCoreSignalLevelListenerBridge::aboutToDelete(AudioCore*)
{
	client.removeListenerBridge(this);
}

void AudioCoreSignalLevelListenerBridge::timerCallback(int timerID)
{
	if (timerID == 0)
	{
		const OwnedArray<Track>& tracks = trackedInstance.getTracks();
		trackLevels.resize(2 * tracks.size());

		const float invertedAveraging = 1 - averaging;

		int levelIndex = 0;
		for (const Track* track : tracks)
		{
			const Array<float>& trackSignalLevels = track->getSignalLevels();
			trackLevels.setUnchecked(levelIndex, trackSignalLevels[0] * averaging + trackLevels[levelIndex] * invertedAveraging);
			++levelIndex;

			trackLevels.setUnchecked(levelIndex, trackSignalLevels[1] * averaging + trackLevels[levelIndex] * invertedAveraging);
			++levelIndex;
		}
		
		const Array<float>& soloLevels = trackedInstance.getSoloBus()->getSignalLevels();
		soloBusLevels.setUnchecked(0, soloLevels[0] * averaging + soloBusLevels[0] * (1 - averaging));
		soloBusLevels.setUnchecked(1, soloLevels[1] * averaging + soloBusLevels[1] * (1 - averaging));
	}
	else
	{
		const int numTracks = fillOutputData();
		client.sendOSCMessage(Messages::AudioCoreSignalLevels(numTracks, tracksSignalLevelsData, soloBusSignalLevelsData, tracksClippingMask));
	}
}

int AudioCoreSignalLevelListenerBridge::fillOutputData()
{
	const uint32 currentTimeMillis = Time::getApproximateMillisecondCounter();
	const OwnedArray<Track>& tracks = trackedInstance.getTracks();

	trackLevels.resize(2 * tracks.size());
	tracksSignalLevelsData.setSize(sizeof(uint16) * 2 * tracks.size());
	tracksClippingMask.setSize((tracks.size() + 7) / 8);
	tracksClippingMask.fillWith(0);
		
	uint16* tracksData = (uint16*)tracksSignalLevelsData.getData();
	char* tracksClipData = (char*)tracksClippingMask.getData();

	int signalLevelSampleIndex = 0;
	int trackIndex = 0;
	for (const Track* track : tracks)
	{
		tracksData[signalLevelSampleIndex] = floatToOutputLevelSample(trackLevels[signalLevelSampleIndex]);
		++signalLevelSampleIndex;

		tracksData[signalLevelSampleIndex] = floatToOutputLevelSample(trackLevels[signalLevelSampleIndex]);
		++signalLevelSampleIndex;

		if (currentTimeMillis - track->getLastClipTimeMilliseconds() < maxClipTimeDiffMs)
			*(tracksClipData + (trackIndex / 8)) |= 1 << (trackIndex % 8);

		++trackIndex;
	}

	uint16* soloBusData = (uint16*)soloBusSignalLevelsData.getData();
	soloBusData[0] = floatToOutputLevelSample(soloBusLevels[0]);
	soloBusData[1] = floatToOutputLevelSample(soloBusLevels[1]);

	return tracks.size();
}

AudioCoreAudioDataListenerBridge::AudioCoreAudioDataListenerBridge(AudioDataEmitter& emitter, RemoteClient& client, int minNumSamples)
	: minimalNumSamples(minNumSamples), OSCBridgeBase(emitter, client)
{
	outputChannelData.setSize(sizeof(int16) * minNumSamples * 4);
}

AudioCoreAudioDataListenerBridge::~AudioCoreAudioDataListenerBridge()
{
	trackedInstance.removeAudioDataListener(this);
}

void AudioCoreAudioDataListenerBridge::audioProcessed(AudioDataEmitter*, const AudioBufferDouble& audioBuffer)
{
	const int numChannels = audioBuffer.getNumChannels();
	const int numSamples = audioBuffer.getNumCurrentSamples();

	int outputDataIndex = outputChannelData.getSize() / sizeof(int16);

	outputNumSamples += numSamples;
	outputChannelData.setSize(outputChannelData.getSize() + (sizeof(int16) * numChannels * numSamples));

	int16* outputData = (int16*)outputChannelData.getData();

	for (int chann = 0; chann < numChannels; ++chann)
	{
		const double* inputChannel = audioBuffer.getReadPointer(chann);
		for (int i = 0; i < numSamples; ++i)
			outputData[outputDataIndex++] = doubleToInt16Sample(inputChannel[i]);
	}

	if (outputNumSamples >= minimalNumSamples)
	{
		client.sendOSCMessage(Messages::AudioCoreAudioData(outputNumSamples, numChannels, outputNumSamples));
		
		outputNumSamples = 0;
		outputChannelData.reset();
	}
}

void AudioCoreAudioDataListenerBridge::aboutToDelete(AudioDataEmitter*)
{
	client.removeListenerBridge(this);
}
