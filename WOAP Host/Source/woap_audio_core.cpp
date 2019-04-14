#include "woap_audio_core.h"
#include "woap_mixer_connection.h"
#include "woap_audio_input_proxy.h"
#include "woap_audio_output_proxy.h"
#include "woap_mixer_source_node.h"
#include "woap_api_core.h"
#include "woap_api_listener_bridges_audio_core_listener_bridge.h"
#include "woap_api_osc_remote_client.h"
#include "woap_api_messages_audio_core.h"
#include "woap_parameter_bool.h"
#include "woap_user_permission.h"
#include "woap_audio_core_processor.h"
#include "woap_solo_bus.h"
#include "woap_track.h"
#include "woap_main.h"

using namespace WOAP;
using namespace WOAP::API;
using namespace WOAP::API::Messages;
using namespace WOAP::API::ListenerBridges;

SINGLETON_IMPLEMENTATION(AudioCore)

AudioCore::AudioCore()
{
	soloBus = new SoloBus(this);
	audioCoreProcessor = new AudioCoreProcessor(this);
	audioDeviceManager = new AudioDeviceManager();

	initializeAPIConsumer();
}

AudioCore::~AudioCore()
{
	
}

const Bootstrapper::Initializer AudioCore::bootstrapInitializer([](Bootstrapper& bootstrapper) {
	bootstrapper.reportPermissions({
		new UserPermission(Permission::ViewNodes, "View nodes"),
		new UserPermission(Permission::ModifyNodes, "Modify nodes")
	});
});

void AudioCore::shutdown()
{
	saveAudioDeviceManagerSettings();

	terminateAPIConsumer();
	stopDevicePlayback();
	removeAllTracks();
	removeAllInputProxies();
	removeAllOutputProxies();

	audioCoreProcessor = nullptr;
	listeners.call(&AudioCore::Listener::aboutToDelete, this);
}

float AudioCore::getGlobalAudioLatencyMs() const
{
	if (audioDeviceManager->getCurrentAudioDevice() == nullptr)
		return -1.0f;
	else
		return (float)globalAudioLatencySamples / (float)audioDeviceManager->getCurrentAudioDevice()->getCurrentSampleRate();
}

void AudioCore::setDefaultMixerSetup()
{
	AudioInputProxy* inputL = addInputProxy("In 1", 0, true);
	AudioInputProxy* inputR = addInputProxy("In 2", 1, true);
	AudioInputProxy* inputS = addInputProxy("In 1 & 2", 0, 1, true);
	addOutputProxy("Out 1", 0, true);
	addOutputProxy("Out 2", 1, true);
	AudioOutputProxy* output = addOutputProxy("Out 1 & 2", 0, 1, true);

	AudioTrack* track1 = addAudioTrack();
	*track1->getPanParameter() = -1.0f;

	AudioTrack* track2 = addAudioTrack();
	*track2->getPanParameter() = 1.0f;

	MixBusTrack* bus1 = addMixBusTrack();

	AudioTrack* track3 = addAudioTrack();

	OutputBusTrack* master = addOutputBusTrack();
	*master->getGainParameter() = -30.0f;

	connect(inputL, track1);
	connect(inputR, track2);

	connect(track1, bus1);
	connect(track2, bus1);

	connect(inputS, track3);
	connect(track3, master);

	connect(bus1, master);
	connect(master, output);
}

void AudioCore::initializeDevicePlayback()
{
	if (audioDevicePlaybackRunning)
		return;

	audioDevicePlaybackRunning = true;
	audioDeviceManager->addAudioCallback(audioCoreProcessor);
	audioCoreProcessor->startProcessing();
}

void AudioCore::stopDevicePlayback()
{
	if (!audioDevicePlaybackRunning)
		return;

	audioDevicePlaybackRunning = false;
	audioDeviceManager->removeAudioCallback(audioCoreProcessor);
	audioCoreProcessor->stopProcessing();
}

void AudioCore::pauseDevicePlayback()
{
	if (!audioDevicePlaybackRunning)
		return;

	audioDevicePlaybackRunning = false;
	audioCoreProcessor->stopProcessing();
}

void AudioCore::resumeDevicePlayback()
{
	if (audioDevicePlaybackRunning)
		return;

	audioDevicePlaybackRunning = true;
	audioCoreProcessor->startProcessing();
}

void AudioCore::selectAudioDevice()
{
	ScopedPointer<XmlElement> config = XmlDocument::parse(Main::getInstanceCast()->getAudioDeviceSettingsFile());
	audioDeviceManager->initialise(2, 2, config, false);
	if (audioDeviceManager->getCurrentAudioDevice() != nullptr) return;

	audioDeviceManager->setCurrentAudioDeviceType("ASIO", true);
	if (audioDeviceManager->getCurrentAudioDevice() != nullptr) return;

	audioDeviceManager->setCurrentAudioDeviceType("Windows Audio", true);
	if (audioDeviceManager->getCurrentAudioDevice() != nullptr) return;

	audioDeviceManager->initialise(0, 2, NULL, false);
	if (audioDeviceManager->getCurrentAudioDevice() != nullptr) return;

	/* Can't initialise audio device */
	jassertfalse;
}

void AudioCore::updateAudioProxiesForCurrentDevice()
{
	ASSERT_ONLY_MESSAGE_THREAD;

	AudioIODevice* device = audioDeviceManager->getCurrentAudioDevice();
	if (device == nullptr)
	{
		jassertfalse;
		return;
	}

	const BigInteger activeInputChannels = device->getActiveInputChannels();
	const BigInteger activeOutputChannels = device->getActiveOutputChannels();

	if (activeInputChannels == lastActiveInputChannels && activeOutputChannels == lastActiveOutputChannels)
		return;

	bool startPlaybackAfterUpdate = false;
	if (isPlaybackRunning())
	{
		startPlaybackAfterUpdate = true;
		pauseDevicePlayback();
	}

	if (activeInputChannels != lastActiveInputChannels)
	{
		removeInputProxiesWithInactiveChannel(activeInputChannels);
		createInputProxiesForActiveChannels(activeInputChannels);

		lastActiveInputChannels = activeInputChannels;
	}

	if (activeOutputChannels != lastActiveOutputChannels)
	{
		removeOutputProxiesWithInactiveChannel(activeOutputChannels);
		createOutputProxiesForActiveChannels(activeOutputChannels);

		lastActiveOutputChannels = activeOutputChannels;
	}
	
	if (startPlaybackAfterUpdate)
		resumeDevicePlayback();
}

void AudioCore::audioDeviceAboutToStart(AudioIODevice* device)
{
	updateAudioProxiesForCurrentDevice();

	playbackActive = true;
	currentBufferSizeSamples = device == nullptr ? 64 : device->getCurrentBufferSizeSamples();
	currentSampleRate = device == nullptr ? 44100 : device->getCurrentSampleRate();

	audioPlaybackInfoListeners.call(&AudioPlaybackInfoListener::playbackAboutToStart, currentSampleRate, currentBufferSizeSamples);
}

void AudioCore::audioDeviceStopped()
{
	playbackActive = false;
	audioPlaybackInfoListeners.call(&AudioPlaybackInfoListener::playbackAboutToStop);
}

void AudioCore::addAudioPlaybackInfoListener(AudioPlaybackInfoListener* listener)
{
	if (playbackActive)
		listener->playbackAboutToStart(currentSampleRate, currentBufferSizeSamples);

	audioPlaybackInfoListeners.add(listener);
}

void AudioCore::removeAudioPlaybackInfoListener(AudioPlaybackInfoListener* listener)
{
	audioPlaybackInfoListeners.remove(listener);
}

Track* AudioCore::addTrack(int trackType, int position)
{
	switch (trackType)
	{
	case AudioTrack::Type:
		return addAudioTrack(position);
	case MixBusTrack::Type:
		return addMixBusTrack(position);
	case FxTrack::Type:
		return addFxTrack(position);
	case OutputBusTrack::Type:
		return addOutputBusTrack(position);
	default:
		jassertfalse;
		return nullptr;
	}
}

AudioTrack* AudioCore::addAudioTrack(int position)
{
	AudioTrack* newTrack = new AudioTrack(NumChannels::Stereo, getNextTrackName(AudioTrack::Type));
	addTrack(newTrack, position);

	return newTrack;
}

FxTrack* AudioCore::addFxTrack(int position)
{
	FxTrack* newTrack = new FxTrack(NumChannels::Stereo, getNextTrackName(FxTrack::Type));
	addTrack(newTrack, position);

	return newTrack;
}

MixBusTrack* AudioCore::addMixBusTrack(int position)
{
	MixBusTrack* newTrack = new MixBusTrack(NumChannels::Stereo, getNextTrackName(MixBusTrack::Type));
	addTrack(newTrack, position);

	return newTrack;
}

OutputBusTrack* AudioCore::addOutputBusTrack(int position)
{
	OutputBusTrack* newTrack = new OutputBusTrack(NumChannels::Stereo, getNextTrackName(OutputBusTrack::Type));
	addTrack(newTrack, position);

	return newTrack;
}

AudioInputProxy* AudioCore::addInputProxy(const String& name, int channelIndex, bool unique)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	
	if (unique)
	{
		AudioInputProxy* p = getAudioInputProxy({ channelIndex });
		if (p != nullptr)
			return p;
	}

	AudioInputProxy* inputProxy = new AudioInputProxy(name, channelIndex);

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		inputProxies.add(inputProxy);
		audioCoreProcessor->audioInputProxyAdded(inputProxy);
	}
	
	listeners.call(&AudioCore::Listener::audioInputProxyAdded, inputProxy);

	return inputProxy;
}

AudioInputProxy* AudioCore::addInputProxy(const String& name, int leftChannelIndex, int rightChannelIndex, bool unique)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (unique)
	{
		AudioInputProxy* p = getAudioInputProxy({ leftChannelIndex, rightChannelIndex });
		if (p != nullptr)
			return p;
	}

	AudioInputProxy* inputProxy = new AudioInputProxy(name, leftChannelIndex, rightChannelIndex);
	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		inputProxies.add(inputProxy);
		audioCoreProcessor->audioInputProxyAdded(inputProxy);
	}

	listeners.call(&AudioCore::Listener::audioInputProxyAdded, inputProxy);

	return inputProxy;
}

void AudioCore::addInputProxy(AudioInputProxy* proxy)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		inputProxies.add(proxy);
		audioCoreProcessor->audioInputProxyAdded(proxy);
	}
	
	listeners.call(&AudioCore::Listener::audioInputProxyAdded, proxy);
}

AudioOutputProxy* AudioCore::addOutputProxy(const String& name, int channelIndex, bool unique)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (unique)
	{
		AudioOutputProxy* p = getAudioOutputProxy({ channelIndex });
		if (p != nullptr)
			return p;
	}

	AudioOutputProxy* outputProxy = new AudioOutputProxy(name, channelIndex);

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		outputProxies.add(outputProxy);
		audioCoreProcessor->audioOutputProxyAdded(outputProxy);
	}
	
	listeners.call(&AudioCore::Listener::audioOutputProxyAdded, outputProxy);

	return outputProxy;
}

AudioOutputProxy* AudioCore::addOutputProxy(const String& name, int leftChannelIndex, int rightChannelIndex, bool unique)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (unique)
	{
		AudioOutputProxy* p = getAudioOutputProxy({ leftChannelIndex, rightChannelIndex });
		if (p != nullptr)
			return p;
	}

	AudioOutputProxy* outputProxy = new AudioOutputProxy(name, leftChannelIndex, rightChannelIndex);

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		outputProxies.add(outputProxy);
		audioCoreProcessor->audioOutputProxyAdded(outputProxy);
	}
	
	listeners.call(&AudioCore::Listener::audioOutputProxyAdded, outputProxy);

	return outputProxy;
}

void AudioCore::addOutputProxy(AudioOutputProxy* proxy)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		outputProxies.add(proxy);
		audioCoreProcessor->audioOutputProxyAdded(proxy);
	}
	
	listeners.call(&AudioCore::Listener::audioOutputProxyAdded, proxy);
}

void AudioCore::removeInputProxy(AudioInputProxy* proxy)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	
	const int index = inputProxies.indexOf(proxy);
	if (index == -1)
	{
		jassertfalse;
		return;
	}

	const ScopedPointer<AudioInputProxy> toDelete(proxy);

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		inputProxies.remove(index, false);
		audioCoreProcessor->audioInputProxyRemoved(proxy);
	}
	
	removeAllConnections(proxy);
	listeners.call(&AudioCore::Listener::audioInputProxyRemoved, proxy);
}

void AudioCore::removeOutputProxy(AudioOutputProxy* proxy)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	const int index = outputProxies.indexOf(proxy);
	if (index == -1)
	{
		jassertfalse;
		return;
	}

	const ScopedPointer<AudioOutputProxy> toDelete(proxy);

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		outputProxies.remove(index, false);
		audioCoreProcessor->audioOutputProxyRemoved(proxy);
	}
	
	removeAllConnections(proxy);
	listeners.call(&AudioCore::Listener::audioOutputProxyRemoved, proxy);
}

void AudioCore::createInputProxiesForActiveChannels(const BigInteger& activeChannels)
{
	const int highestActiveChannel = activeChannels.getHighestBit();

	BigInteger monoProxies;
	BigInteger stereoProxies;
	for (AudioInputProxy* proxy : inputProxies)
	{
		const Array<int>& channs = proxy->getAssignedChannels();
		switch (channs.size())
		{
		case 1:
			monoProxies.setBit(channs[0]);
			break;
		case 2:
			stereoProxies.setBit(channs[0]);
			stereoProxies.setBit(channs[1]);
		default:
			break;
		}
	}

	for (int chann = 0; chann <= highestActiveChannel; chann = chann + 2)
	{
		const int leftChannel = chann;
		const int rightChannel = chann + 1;

		if (activeChannels[leftChannel] && !monoProxies[leftChannel])
		{
			addInputProxy("In " + String(leftChannel + 1), leftChannel);
		}

		if (activeChannels[rightChannel] && !monoProxies[rightChannel])
		{
			addInputProxy("In " + String(rightChannel + 1), rightChannel);
		}

		if (activeChannels[leftChannel] && activeChannels[rightChannel] && !stereoProxies[leftChannel] && !stereoProxies[rightChannel])
		{
			addInputProxy("In " + String(leftChannel + 1) + " & " + String(rightChannel + 1), leftChannel, rightChannel);
		}
	}
}

void AudioCore::createOutputProxiesForActiveChannels(const BigInteger& activeChannels)
{
	const int highestActiveChannel = activeChannels.getHighestBit();

	BigInteger monoProxies;
	BigInteger stereoProxies;
	for (AudioOutputProxy* proxy : outputProxies)
	{
		const Array<int>& channs = proxy->getAssignedChannels();
		switch (channs.size())
		{
		case 1:
			monoProxies.setBit(channs[0]);
			break;
		case 2:
			stereoProxies.setBit(channs[0]);
			stereoProxies.setBit(channs[1]);
		default:
			break;
		}
	}

	for (int chann = 0; chann <= highestActiveChannel; chann = chann + 2)
	{
		const int leftChannel = chann;
		const int rightChannel = chann + 1;

		if (activeChannels[leftChannel] && !monoProxies[leftChannel])
		{
			addOutputProxy("Out " + String(leftChannel + 1), leftChannel);
		}

		if (activeChannels[rightChannel] && !monoProxies[rightChannel])
		{
			addOutputProxy("Out " + String(rightChannel + 1), rightChannel);
		}

		if (activeChannels[leftChannel] && activeChannels[rightChannel] && !stereoProxies[leftChannel] && !stereoProxies[rightChannel])
		{
			addOutputProxy("Out " + String(leftChannel + 1) + " & " + String(rightChannel + 1), leftChannel, rightChannel);
		}
	}
}

void AudioCore::removeAllInputProxies()
{
	ASSERT_ONLY_MESSAGE_THREAD;
	const ScopedLock sl(audioCoreProcessor->getProcessLock());

	for (int i = inputProxies.size() - 1; i >= 0; --i)
	{
		ScopedPointer<AudioInputProxy> proxy(inputProxies.removeAndReturn(i));
		removeAllConnections(proxy);

		audioCoreProcessor->audioInputProxyRemoved(proxy);
		listeners.call(&AudioCore::Listener::audioInputProxyRemoved, proxy);
	}
}

void AudioCore::removeInputProxiesWithInactiveChannel(const BigInteger& channels)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	Array<AudioInputProxy*> proxiesToRemove;
	for (AudioInputProxy* proxy : inputProxies)
	{
		for (const int chann : proxy->getAssignedChannels())
		{
			if (!channels[chann])
			{
				proxiesToRemove.add(proxy);
				break;
			}
		}
	}

	for (AudioInputProxy* proxy : proxiesToRemove)
		removeInputProxy(proxy);
}

void AudioCore::removeAllOutputProxies()
{
	ASSERT_ONLY_MESSAGE_THREAD;
	const ScopedLock sl(audioCoreProcessor->getProcessLock());

	for (int i = outputProxies.size() - 1; i >= 0; --i)
	{
		ScopedPointer<AudioOutputProxy> proxy(outputProxies.removeAndReturn(i));
		removeAllConnections(proxy);

		audioCoreProcessor->audioOutputProxyRemoved(proxy);
		listeners.call(&AudioCore::Listener::audioOutputProxyRemoved, proxy);
	}
}

void AudioCore::removeOutputProxiesWithInactiveChannel(const BigInteger& channels)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	Array<AudioOutputProxy*> proxiesToRemove;
	for (AudioOutputProxy* proxy : outputProxies)
	{
		for (const int chann : proxy->getAssignedChannels())
		{
			if (!channels[chann])
			{
				proxiesToRemove.add(proxy);
				break;
			}
		}
	}

	for (AudioOutputProxy* proxy : proxiesToRemove)
		removeOutputProxy(proxy);
}

Track* AudioCore::getTrack(const Uuid& trackId) const
{
	for (Track* track : tracks)
	{
		if (track->getId() == trackId)
			return track;
	}

	return nullptr;
}

MixerSourceNode* AudioCore::getSourceNode(const Uuid& nodeId) const
{
	for (AudioInputProxy* proxy : inputProxies)
	{
		if (proxy->getId() == nodeId)
			return proxy;
	}

	for (Track* track : tracks)
	{
		if (track->getId() == nodeId)
			return track;

		for (TrackSendNode* send : track->getSends())
			if (send->getId() == nodeId)
				return send;
	}

	if (soloBus->getId() == nodeId)
		return soloBus;

	return nullptr;
}

MixerReceiverNode* AudioCore::getReceiverNode(const Uuid& nodeId) const
{
	for (Track* track : tracks)
	{
		if (track->getId() == nodeId)
			return track;
	}

	for (AudioOutputProxy* proxy : outputProxies)
	{
		if (proxy->getId() == nodeId)
			return proxy;
	}

	return nullptr;
}

MixerNode* AudioCore::getNode(const Uuid& nodeId) const
{
	for (AudioInputProxy* proxy : inputProxies)
	{
		if (proxy->getId() == nodeId)
			return proxy;
	}

	for (Track* track : tracks)
	{
		if (track->getId() == nodeId)
			return track;
	}

	for (AudioOutputProxy* proxy : outputProxies)
	{
		if (proxy->getId() == nodeId)
			return proxy;
	}

	if (soloBus->getId() == nodeId)
		return soloBus;

	return nullptr;
}

void AudioCore::removeAllNodes()
{
	removeAllTracks();
	removeAllInputProxies();
	removeAllOutputProxies();

	jassert(tracks.isEmpty());
	jassert(inputProxies.isEmpty());
	jassert(outputProxies.isEmpty());
	jassert(connections.isEmpty());
}

int AudioCore::getNumNodes() const
{
	return inputProxies.size() + tracks.size() + outputProxies.size() + 1;
}

void AudioCore::addTrack(Track* newTrack, int position)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		tracks.insert(position, newTrack);
		audioCoreProcessor->trackAdded(newTrack, position);
	}
	
	listeners.call(&AudioCore::Listener::trackAdded, newTrack, position);
}

void AudioCore::removeTrack(const Uuid& trackId)
{
	removeTrack(getTrack(trackId));
}

void AudioCore::removeTrack(Track* track)
{
	if (track == nullptr) 
	{
		jassertfalse;
		return;
	}

	ASSERT_ONLY_MESSAGE_THREAD;
	int index = tracks.indexOf(track);
	if (index == -1)
	{
		jassertfalse;
		return;
	}

	const ScopedPointer<Track> toDelete(track);

	removeAllConnections(track);

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		tracks.remove(index, false);
		audioCoreProcessor->trackRemoved(track);
	}

	listeners.call(&AudioCore::Listener::trackRemoved, track);
}

void AudioCore::removeAllTracks()
{
	ASSERT_ONLY_MESSAGE_THREAD;

	const ScopedLock sl(audioCoreProcessor->getProcessLock());
	for (int i = tracks.size() - 1; i >= 0; --i)
	{
		ScopedPointer<Track> track(tracks.removeAndReturn(i));
		removeAllConnections(track);

		audioCoreProcessor->trackRemoved(track);
		listeners.call(&AudioCore::Listener::trackRemoved, track);
	}
}

void AudioCore::moveTrack(int oldPosition, int newPosition)
{
	if (oldPosition >= tracks.size())
	{
		jassertfalse;
		return;
	}
	
	moveTrack(tracks[oldPosition], newPosition);
}

void AudioCore::moveTrack(const Uuid& trackId, int newPosition)
{
	moveTrack(getTrack(trackId), newPosition);
}

void AudioCore::moveTrack(Track* track, int newPosition)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (track == nullptr) 
	{
		jassertfalse;
		return;
	}

	const int oldPosition = tracks.indexOf(track);

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		tracks.move(oldPosition, newPosition);
		audioCoreProcessor->trackMoved(track, oldPosition, newPosition);
	}
	
	listeners.call(&AudioCore::Listener::trackMoved, track, oldPosition, newPosition);
}

AudioInputProxy* AudioCore::getAudioInputProxy(const Array<int>& channelIndexes) const
{
	for (AudioInputProxy* proxy : inputProxies)
	{
		if (proxy->getAssignedChannels() == channelIndexes)
			return proxy;
	}

	return nullptr;
}

AudioOutputProxy* AudioCore::getAudioOutputProxy(const Array<int>& channelIndexes) const
{
	for (AudioOutputProxy* proxy : outputProxies)
	{
		if (proxy->getAssignedChannels() == channelIndexes)
			return proxy;
	}

	return nullptr;
}

int AudioCore::getNumTracks() const
{
	return tracks.size();
}

int AudioCore::getNumTracks(int trackType) const
{
	int output = 0;

	for (Track* track : tracks)
	{
		if (track->getType() == trackType)
		{
			++output;
		}
	}

	return output;
}

int AudioCore::getNextTrackNumber(int trackType) const
{
	Array<int> trackNumbers;
	for (Track* track : tracks)
	{
		if (track->getType() == trackType)
			trackNumbers.addUsingDefaultSort(getIntFromStringEnd(track->getName()));
	}

	return getFirstMissingNumber(trackNumbers, 1);
}

String AudioCore::getNextTrackName(int trackType) const
{
	switch (trackType)
	{
	case FxTrack::Type:
		return String("Fx track " + String(getNextTrackNumber(trackType)));
	case MixBusTrack::Type:
		return String("Mix bus " + String(getNextTrackNumber(trackType)));
	case OutputBusTrack::Type:
		return String("Master " + String(getNextTrackNumber(trackType)));
	case AudioTrack::Type:
	default:
		return String("Track " + String(getNextTrackNumber(trackType)));
	}
}

Array<MixerSourceNode*> AudioCore::getAvailableInputSources(const Uuid& receiverId) const
{
	return getAvailableInputSources(getReceiverNode(receiverId));
}

Array<MixerSourceNode*> AudioCore::getAvailableInputSources(const MixerReceiverNode* receiver) const
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (receiver == nullptr) 
	{
		jassertfalse;
		return Array<MixerSourceNode*>();
	}

	Array<MixerSourceNode*> sources;

	for (AudioInputProxy* source : inputProxies)
	{
		if (receiver->canConnectInputSource(source) || receiver->hasInputSourceAssigned(source, false))
			sources.add(source);
	}

	for (Track* track : tracks)
	{
		if (receiver->canConnectInputSource(track) || receiver->hasInputSourceAssigned(track, false))
			sources.add(track);

		for (TrackSendNode* send : track->getSends())
		{
			if (receiver->canConnectInputSource(send) || receiver->hasInputSourceAssigned(send, false))
				sources.add(send);
		}
	}

	return sources;
}

Array<MixerReceiverNode*> AudioCore::getAvailableOutputReceivers(const Uuid& sourceId) const
{
	return getAvailableOutputReceivers(getSourceNode(sourceId));
}

Array<MixerReceiverNode*> AudioCore::getAvailableOutputReceivers(const MixerSourceNode* source) const
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (source == nullptr) 
	{
		jassertfalse;
		return Array<MixerReceiverNode*>();
	}
	
	Array<MixerReceiverNode*> receivers;

	for (AudioOutputProxy* receiver : outputProxies)
	{
		if (source->canConnectOutputReceiver(receiver) || source->hasOutputReceiverAssigned(receiver, false))
			receivers.add(receiver);
	}

	for (Track* track : tracks)
	{
		if (source->canConnectOutputReceiver(track) || source->hasOutputReceiverAssigned(track, false))
			receivers.add(track);
	}

	return receivers;
}

void AudioCore::recountGlobalTreeLatency()
{
	ASSERT_ONLY_AUDIO_PROCESS_THREADS;

	int maxTreeLatency = 0;
	for (const AudioInputProxy* proxy : inputProxies)
	{
		maxTreeLatency = jmax(maxTreeLatency, proxy->getLatencySamplesInTree());
	}

	if (maxTreeLatency != globalAudioLatencySamples)
	{
		globalAudioLatencySamples = maxTreeLatency;

		for (AudioInputProxy* proxy : inputProxies)
		{
			proxy->setGlobalAudioLatencySamples(globalAudioLatencySamples);
		}

		const float latencyMs = getGlobalAudioLatencyMs();

		audioCoreProcessor->globalAudioLatencyChanged(globalAudioLatencySamples, latencyMs);
		listeners.call(&Listener::globalAudioLatencyChanged, globalAudioLatencySamples, latencyMs);
	}
	
	for (Track* track : tracks)
	{
		track->treeLatencyChanged();
	}
}

const CriticalSection& AudioCore::getProcessLock() const
{
	return audioCoreProcessor->getProcessLock();
}

bool AudioCore::isAudioProcessorThread(Thread::ThreadID id) const
{
	return audioCoreProcessor->isAudioProcessorThread(id);
}

bool AudioCore::connect(const Uuid& sourceId, const Uuid& receiverId)
{	
	return connect(getSourceNode(sourceId), getReceiverNode(receiverId));
}

bool AudioCore::connect(MixerSourceNode* source, MixerReceiverNode* receiver)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (source == nullptr || receiver == nullptr) 
	{
		jassertfalse;
		return false;
	}

	jassert(source->canConnectOutputReceiver(receiver));
	jassert(receiver->canConnectInputSource(source));

	if (!source->canConnectOutputReceiver(receiver))
		return false;

	if (isConnected(source, receiver))
	{
		jassertfalse;
		return false;
	}

	{
		const ScopedLock sl(audioCoreProcessor->getProcessLock());
		connections.add(new MixerConnection(source, receiver));
		audioCoreProcessor->connectionsChanged();
	}

	listeners.call(&Listener::connectionsChanged);

	return true;
}

bool AudioCore::isConnected(MixerSourceNode* source, MixerReceiverNode* receiver) const
{
	for (const MixerConnection* connection : connections)
		if (connection->isConnecting(source, receiver))
			return true;

	return false;
}

void AudioCore::removeConnection(const Uuid& sourceId, const Uuid& receiverId, bool notifyListeners)
{
	return removeConnection(getSourceNode(sourceId), getReceiverNode(receiverId), notifyListeners);
}

void AudioCore::removeConnection(MixerSourceNode* source, MixerReceiverNode* receiver, bool notifyListeners)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (source == nullptr || receiver == nullptr) 
	{
		jassertfalse;
		return;
	}

	ScopedPointer<MixerConnection> connectionToRemove;
	for (MixerConnection* connection : connections)
	{
		if (connection->isConnecting(source, receiver))
		{
			connectionToRemove = connection;
			connections.removeObject(connection, false);
			break;
		}
	}

	if (notifyListeners)
	{
		{
			const ScopedLock sl(audioCoreProcessor->getProcessLock());
			connectionToRemove = nullptr;
			audioCoreProcessor->connectionsChanged();
		}

		listeners.call(&AudioCore::Listener::connectionsChanged);
	}
}

void AudioCore::removeAllConnections(Track* track, bool notifyListeners)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	removeAllConnections(static_cast<MixerSourceNode*>(track), notifyListeners);
	removeAllConnections(static_cast<MixerReceiverNode*>(track), notifyListeners);

	for (TrackSendNode* send : track->getSends())
	{
		removeAllConnections(static_cast<MixerSourceNode*>(send), notifyListeners);
	}
}

void AudioCore::removeAllConnections(TrackSendNode* send, bool notifyListeners)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	removeAllConnections(static_cast<MixerSourceNode*>(send), notifyListeners);
}

void AudioCore::removeAllConnections(MixerSourceNode* node, bool notifyListeners)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	Array<MixerReceiverNode*> receivers = node->getOutputReceivers();
	for (MixerReceiverNode* receiver : receivers)
	{
		removeConnection(node, receiver, notifyListeners);
	}
}

void AudioCore::removeAllConnections(MixerReceiverNode* node, bool notifyListeners)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	Array<MixerSourceNode*> sources = node->getInputSources();
	for (MixerSourceNode* source : sources)
	{
		removeConnection(source, node, notifyListeners);
	}
}

Array<AddressWithTypeId> AudioCore::getMessageTypesToListenTo() const
{
	return Array<AddressWithTypeId>({
		AddressWithTypeId(AudioCoreAddTrack::getAddress(), MessageType::AddTrack),
		AddressWithTypeId(AudioCoreRemoveTrack::getAddress(), MessageType::RemoveTrack),
		AddressWithTypeId(AudioCoreMoveTrack::getAddress(), MessageType::MoveTrack),
		AddressWithTypeId(AudioCoreConnectNodes::getAddress(), MessageType::ConnectNodes),
		AddressWithTypeId(AudioCoreRemoveNodesConnection::getAddress(), MessageType::RemoveNodesConnection),
		AddressWithTypeId(AudioCoreGetTracksList::getAddress(), MessageType::GetTracksList),
		AddressWithTypeId(AudioCoreGetInputProxiesList::getAddress(), MessageType::GetInputProxiesList),
		AddressWithTypeId(AudioCoreGetOutputProxiesList::getAddress(), MessageType::GetOutputProxiesList),
		AddressWithTypeId(AudioCoreGetAvailableInputSourcesList::getAddress(), MessageType::GetAvailableInputSourcesList),
		AddressWithTypeId(AudioCoreGetAvailableOutputReceiversList::getAddress(), MessageType::GetAvailableOutputReceiversList),
		AddressWithTypeId(AudioCoreGetGlobalAudioLatency::getAddress(), MessageType::GetGlobalAudioLatency),
		AddressWithTypeId(AudioCoreAddListener::getAddress(), MessageType::AddListener),
		AddressWithTypeId(AudioCoreRemoveListener::getAddress(), MessageType::RemoveListener),
		AddressWithTypeId(AudioCoreAddSignalLevelListener::getAddress(), MessageType::AddSignalLevelListener),
		AddressWithTypeId(AudioCoreRemoveSignalLevelListener::getAddress(), MessageType::RemoveSignalLevelListener)
	});
}

void AudioCore::messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId)
{
	switch (messageTypeId)
	{
	case MessageType::AddTrack:
	{
		AudioCoreAddTrack msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyNodes))
			return;

		addTrack(msg.getTrackType(), msg.getTrackPosition());
		break;
	}
	case MessageType::RemoveTrack:
	{
		AudioCoreRemoveTrack msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyNodes))
			return;
		
		removeTrack(msg.getTrackId());
		break;
	}
	case MessageType::MoveTrack:
	{
		AudioCoreMoveTrack msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyNodes))
			return;

		moveTrack(msg.getTrackId(), msg.getNewPosition());
		break;
	}
	case MessageType::ConnectNodes:
	{
		AudioCoreConnectNodes msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyNodes))
			return;

		connect(msg.getSourceNodeId(), msg.getReceiverNodeId());
		break;
	}
	case MessageType::RemoveNodesConnection:
	{
		AudioCoreRemoveNodesConnection msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyNodes))
			return;

		removeConnection(msg.getSourceNodeId(), msg.getReceiverNodeId());
		break;
	}
	case MessageType::GetTracksList:
	{
		AudioCoreGetTracksList msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ViewNodes))
			return;

		StringArray responseData;
		for (Track* track : tracks)
		{
			responseData.add(track->getId().toDashedString());
		}

		client.sendOSCMessage(AudioCoreTracksList(msg, responseData));
		break;
	}
	case MessageType::GetInputProxiesList:
	{
		AudioCoreGetInputProxiesList msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ViewNodes))
			return;

		StringArray responseData;
		for (AudioInputProxy* proxy : inputProxies)
		{
			responseData.add(proxy->getId().toDashedString());
		}

		client.sendOSCMessage(AudioCoreInputProxiesList(msg, responseData));
		break;
	}
	case MessageType::GetOutputProxiesList:
	{
		AudioCoreGetOutputProxiesList msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ViewNodes))
			return;

		StringArray responseData;
		for (AudioOutputProxy* proxy : outputProxies)
		{
			responseData.add(proxy->getId().toDashedString());
		}

		client.sendOSCMessage(AudioCoreOutputProxiesList(msg, responseData));
		break;
	}
	case MessageType::GetAvailableInputSourcesList:
	{
		AudioCoreGetAvailableInputSourcesList msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ViewNodes))
			return;

		MixerReceiverNode* receiver = getReceiverNode(msg.getReceiverNodeId());
		Array<MixerSourceNode*> nodes = getAvailableInputSources(receiver);

		client.sendOSCMessage(AudioCoreAvailableInputSourcesList(msg, nodes, receiver));
		break;
	}
	case MessageType::GetAvailableOutputReceiversList:
	{
		AudioCoreGetAvailableOutputReceiversList msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ViewNodes))
			return;

		MixerSourceNode* source = getSourceNode(msg.getSourceNodeId());
		Array<MixerReceiverNode*> nodes = getAvailableOutputReceivers(source);

		client.sendOSCMessage(AudioCoreAvailableOutputReceiversList(msg, nodes, source));
		break;
	}
	case MessageType::GetGlobalAudioLatency:
	{
		AudioCoreGetGlobalAudioLatency msg(message);
		if (!testValidity(client, msg))
			return;

		client.sendOSCMessage(AudioCoreGlobalAudioLatency(msg, getGlobalAudioLatencySamples(), getGlobalAudioLatencyMs()));
		break;
	}
	case MessageType::AddListener:
	{
		AudioCoreAddListener msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ViewNodes))
			return;

		AudioCoreListenerBridge::addToList<AudioCore::Listener>(listeners, new AudioCoreListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveListener:
	{
		AudioCoreRemoveListener msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ViewNodes))
			return;

		AudioCoreListenerBridge::removeFromList(listeners, *this, client);
		break;
	}
	case MessageType::AddSignalLevelListener:
	{
		AudioCoreAddSignalLevelListener msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ViewNodes))
			return;

		if (msg.getRefreshRateHz() <= 0) {
			AudioCoreSignalLevelListenerBridge::removeFromList(signalLevelListeners, *this, client);
			return;
		}

		AudioCoreSignalLevelListenerBridge::addToList<AudioCore::SignalLevelListener>(signalLevelListeners, new AudioCoreSignalLevelListenerBridge(*this, client, msg.getRefreshRateHz()));
		break;
	}
	case MessageType::RemoveSignalLevelListener:
	{
		AudioCoreRemoveSignalLevelListener msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ViewNodes))
			return;

		AudioCoreSignalLevelListenerBridge::removeFromList(signalLevelListeners, *this, client);
		break;
	}
	default:
		handleUnknownMessage("AudioCore", client, message, messageTypeId);
		break;
	}
}

ValueTree AudioCore::getState() const
{
	ValueTree state("AUDIOCORE");

	ValueTree inputProxiesState("INPUTPROXIES");
	for (AudioInputProxy* inputProxy : inputProxies)
	{
		inputProxiesState.addChild(inputProxy->getState(), -1, nullptr);
	}
	state.addChild(inputProxiesState, -1, nullptr);

	ValueTree outputProxiesState("OUTPUTPROXIES");
	for (AudioOutputProxy* outputProxy : outputProxies)
	{
		outputProxiesState.addChild(outputProxy->getState(), -1, nullptr);
	}
	state.addChild(outputProxiesState, -1, nullptr);

	ValueTree tracksState("TRACKS");
	for (Track* track : tracks)
	{
		tracksState.addChild(track->getState(), -1, nullptr);
	}
	state.addChild(tracksState, -1, nullptr);

	state.addChild(soloBus->getState(), -1, nullptr);

	ValueTree connectionsState("CONNECTIONS");
	for (MixerConnection* connection : connections)
	{
		ValueTree connState("CONNECTION");
		StateHelper::setIdProperty(connState, "source", connection->getInputSource());
		StateHelper::setIdProperty(connState, "receiver", connection->getOutputReceiver());
		connectionsState.addChild(connState, -1, nullptr);
	}
	state.addChild(connectionsState, -1, nullptr);

	return state;
}

void AudioCore::setState(const ValueTree& state)
{
	if (!state.hasType("AUDIOCORE"))
		return;

	bool startPlaybackAfterStateSet = false;
	if (isPlaybackRunning()) 
	{
		stopDevicePlayback();
		startPlaybackAfterStateSet = true;
	}
	
	removeAllNodes();

	for (ValueTree inputProxyState : state.getChildWithName("INPUTPROXIES"))
	{
		AudioInputProxy* proxy = AudioInputProxy::fromState(inputProxyState);
		if (proxy != nullptr)
			addInputProxy(proxy);
	}

	for (ValueTree outputProxyState : state.getChildWithName("OUTPUTPROXIES"))
	{
		AudioOutputProxy* proxy = AudioOutputProxy::fromState(outputProxyState);
		if (proxy != nullptr)
			addOutputProxy(proxy);
	}
	
	for (ValueTree trackState : state.getChildWithName("TRACKS"))
	{
		Track* track = Track::fromState(trackState);
		if (track != nullptr)
			addTrack(track);
	}
	
	soloBus->setState(state.getChildWithName("SOLOBUS"));

	for (ValueTree connectionState : state.getChildWithName("CONNECTIONS"))
	{
		if (!connectionState.hasType("CONNECTION"))
			continue;

		connect(
			StateHelper::getIdProperty(connectionState, "source"),
			StateHelper::getIdProperty(connectionState, "receiver")
		);
	}
	
	if (startPlaybackAfterStateSet)
		initializeDevicePlayback();
}

void AudioCore::saveAudioDeviceManagerSettings()
{
	ScopedPointer<XmlElement> state = audioDeviceManager->createStateXml();
	if (state == nullptr)
		return;

	if (!state->writeToFile(Main::getInstanceCast()->getAudioDeviceSettingsFile(), ""))
	{
		DBG("Audio device write failed");
	}
}