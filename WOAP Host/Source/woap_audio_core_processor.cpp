#include "woap_audio_core_processor.h"
#include "woap_mixer_connection.h"
#include "woap_audio_input_proxy.h"
#include "woap_audio_output_proxy.h"
#include "woap_solo_bus.h"

using namespace WOAP;

AudioCoreProcessor::AudioCoreProcessor(AudioCore* core)
	: audioCore(core)
{
	for (Track* tr : core->getTracks()) { trackAdded(tr, -1); }
	for (AudioInputProxy* pr : core->getAudioInputProxies()) { audioInputProxyAdded(pr); }
	for (AudioOutputProxy* pr : core->getAudioOutputProxies()) { audioOutputProxyAdded(pr); }
	soloBus = core->getSoloBus();

	AudioCoreNodeProcessingState* soloBusState = new AudioCoreNodeProcessingState();
	soloBusState->processJob = new SoloBusProcessJob(this, soloBus);
	soloBus->setProcessingState(soloBusState);
	soloBus->setProcessLock(processLock);
	mixerNodes.add(soloBus);

	const int numWorkers = SystemStats::getNumCpus();
	for (int i = 0; i < numWorkers; ++i)
	{
		processWorkers.add(new Worker(this, i));
	}
}

AudioCoreProcessor::~AudioCoreProcessor()
{
	stopProcessing();
	for (Track* tr : tracks) { tr->removePluginsListener(this); }
}

void AudioCoreProcessor::startProcessing()
{
	const ScopedLock sl(processLock);
	processingShouldStop = false;

	for (Worker* worker : processWorkers)
	{
		worker->startThread(Thread::realtimeAudioPriority);
	}
}

void AudioCoreProcessor::stopProcessing()
{
	processingShouldStop = true;
	const ScopedLock sl(processLock);

	jobsQueue.clear();

	for (Worker* worker : processWorkers)
	{
		worker->stopThread(500);
	}
}

void AudioCoreProcessor::audioDeviceIOCallback(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples)
{
	if (processingShouldStop)
		return;

#ifdef JUCE_DEBUG
	AudioBuffer<float> debugData;
	const bool debugSignal = false;
	if (debugSignal)
	{
		numInputChannels = 2;
		fillDebugSamples(debugData, numInputChannels, numSamples);
		inputChannelData = debugData.getArrayOfReadPointers();
	}

	{
		const ScopedTryLock sl(processLock);
		if (!sl.isLocked())
			DBG("Audio enter lag");
	}

	const double processingTimeStart = Time::getMillisecondCounterHiRes();
#endif
	const ScopedLock sl(processLock);
	if (nodesToProcess != 0)
	{
		/* Pass to prevent extreme overload */
		DBG("Audio overload");

		AudioBuffer<float> outputData(outputChannelData, numOutputChannels, numSamples);
		outputData.clear();
		return;
	}

	processThreadId = Thread::getCurrentThreadId();

	processingArguments.inputChannelData = inputChannelData;
	processingArguments.numInputChannels = numInputChannels;
	processingArguments.outputChannelData = outputChannelData;
	processingArguments.numOutputChannels = numOutputChannels;
	processingArguments.numSamples = numSamples;
	processingArguments.jobsCanBePoped = false;

	if (schemaLatencyDirty)
	{
		recountGlobalTreeLatency();
		recountTrackInputLatencies();
		schemaLatencyDirty = false;
	}

	if (schemaDirty)
	{
		recountNodesSchemaCache();
		jobsQueue.resize(audioCore->getNumNodes() * 2);
		schemaDirty = false;
	}

	nodesToProcess = mixerNodes.size();
	jobsQueue.clear();
	nodesReadyBeforeProcessing.clearQuick();

	AudioBuffer<float> outputs(outputChannelData, numOutputChannels, numSamples);
	outputs.clear();

	static_cast<AudioCoreNodeProcessingState*>(soloBus->getProcessingState())->numInputNodes = soloBus->getSoloedTracks().size();
	for (MixerNode* node : mixerNodes)
	{
		AudioCoreNodeProcessingState* const state = static_cast<AudioCoreNodeProcessingState*>(node->getProcessingState());
		state->numInputNodesProcessed = 0;
		state->processed = false;

		if (state->canBeProcessed())
			nodesReadyBeforeProcessing.add(node);
	}
	
	for (MixerNode* node : nodesReadyBeforeProcessing)
		nodeReady(node, false);

	processingArguments.jobsCanBePoped = true;
	
	notifyAllWorkers();

#ifdef JUCE_DEBUG
	if (!workerSyncEvent.wait(2000))
	{
		DBG("Audio critical lag");
	}
	else if (Time::getMillisecondCounterHiRes() - processingTimeStart > audioCallbackMaxTimeMs)
	{
		const double longerRatio = (Time::getMillisecondCounterHiRes() - processingTimeStart) / audioCallbackMaxTimeMs;
		DBG("Audio processing took " + String(longerRatio, 2) + " times longer");
	}
	
	for (MixerNode* node : mixerNodes)
	{
		if (static_cast<AudioCoreNodeProcessingState*>(node->getProcessingState())->processed == false)
			DBG(node->getName() + " was left unprocessed");
	}

	if (!processingShouldStop && nodesToProcess != 0)
	{
		DBG(String(nodesToProcess) + " nodes were left unprocessed");
	}
#else
	workerSyncEvent.wait(2000);
#endif
}

void AudioCoreProcessor::audioDeviceAboutToStart(AudioIODevice* device)
{
	audioCallbackMaxTimeMs = 1000 * (device->getCurrentBufferSizeSamples() / device->getCurrentSampleRate());
	audioCore->audioDeviceAboutToStart(device);
}

void AudioCoreProcessor::audioDeviceStopped()
{
	audioCore->audioDeviceStopped();
}

void AudioCoreProcessor::audioInputProxyAdded(AudioInputProxy* proxy)
{
	jassert(proxy->getProcessingState() == nullptr);
	AudioCoreNodeProcessingState* state = new AudioCoreNodeProcessingState();
	state->processJob = new InputProxyProcessJob(this, proxy);
	proxy->setProcessingState(state);
	proxy->setProcessLock(processLock);

	inputProxies.add(proxy);
	mixerNodes.add(proxy);
	setSchemaDirty();
}

void AudioCoreProcessor::audioOutputProxyAdded(AudioOutputProxy* proxy)
{
	jassert(proxy->getProcessingState() == nullptr);
	AudioCoreNodeProcessingState* state = new AudioCoreNodeProcessingState();
	state->processJob = new OutputProxyProcessJob(this, proxy);
	proxy->setProcessingState(state);
	proxy->setProcessLock(processLock);

	outputProxies.add(proxy);
	mixerNodes.add(proxy);
	setSchemaDirty();
}

void AudioCoreProcessor::audioInputProxyRemoved(AudioInputProxy* proxy)
{
	inputProxies.removeFirstMatchingValue(proxy);
	mixerNodes.removeFirstMatchingValue(proxy);
	setSchemaDirty();
}

void AudioCoreProcessor::audioOutputProxyRemoved(AudioOutputProxy* proxy)
{
	outputProxies.removeFirstMatchingValue(proxy);
	mixerNodes.removeFirstMatchingValue(proxy);
	setSchemaDirty();
}

void AudioCoreProcessor::trackAdded(Track* track, int)
{
	jassert(track->getProcessingState() == nullptr);

	track->addPluginsListener(this);

	TrackProcessingState* state = new TrackProcessingState();
	state->processJob = new TrackProcessJob(this, track);
	track->setProcessingState(state);
	track->setProcessLock(processLock);

	for (TrackSendNode* send : track->getSends())
		sendAdded(send, -1);

	tracks.add(track);
	mixerNodes.add(track);
	setSchemaDirty();
}

void AudioCoreProcessor::trackRemoved(Track* track)
{
	track->removePluginsListener(this);

	for (TrackSendNode* send : track->getSends())
		sendRemoved(send);

	tracks.removeFirstMatchingValue(track);
	mixerNodes.removeFirstMatchingValue(track);
	setSchemaDirty();
}

void AudioCoreProcessor::connectionsChanged()
{
	setSchemaDirty();
}

void AudioCoreProcessor::aboutToDelete(AudioCore*)
{
	audioCore->removeListener(this);
}

void AudioCoreProcessor::pluginAdded(AudioProcessorPlayerDouble*, int)
{
	setSchemaLatencyDirty();
}

void AudioCoreProcessor::pluginRemoved(AudioProcessorPlayerDouble*)
{
	setSchemaLatencyDirty();
}

void AudioCoreProcessor::pluginMoved(AudioProcessorPlayerDouble*, int, int)
{
	setSchemaLatencyDirty();
}

void AudioCoreProcessor::sendAudioSourcePositionChanged(TrackSendNode*)
{
	setSchemaLatencyDirty();
}

void AudioCoreProcessor::sendAdded(TrackSendNode* node, int)
{
	node->addAudioSourcePositionListener(this);
	node->setProcessLock(processLock);
}

void AudioCoreProcessor::sendRemoved(TrackSendNode* node)
{
	node->removeAudioSourcePositionListener(this);
}

void AudioCoreProcessor::sendMoved(TrackSendNode*, int, int)
{
	
}

void AudioCoreProcessor::fillDebugSamples(AudioBuffer<float>& bufferToFill, int numChannels, int numSamples) const
{
	bufferToFill.setSize(numChannels, numSamples);
	float** input = bufferToFill.getArrayOfWritePointers();

	static float debugSample;
	for (int chann = 0; chann < numChannels; ++chann)
	{
		debugSample = 0;

		for (int i = 0; i < numSamples; ++i)
		{
			input[chann][i] = debugSample;
			debugSample += 0.001f;
		}
	}
}

void AudioCoreProcessor::recountGlobalTreeLatency() const
{
	audioCore->recountGlobalTreeLatency();
}

void AudioCoreProcessor::recountTrackInputLatencies() const
{
	for (const Track* track : tracks)
	{
		TrackProcessingState* const state = static_cast<TrackProcessingState*>(track->getProcessingState());
		state->inputLatencySamples.resize(track->getInputConnections().size());
		state->inputLatencySamples.clearQuick();

		for (const MixerConnection* connection : track->getInputConnections())
		{
			state->inputLatencySamples.add(connection->getInputSource()->getLatencyDiffForNode(track));
		}
	}
}

void AudioCoreProcessor::recountNodesSchemaCache() const
{
	for (Track* track : tracks)
	{
		TrackProcessingState* const state = static_cast<TrackProcessingState*>(track->getProcessingState());

		state->numInputNodes = track->getInputConnections().size();
		state->outputReceivers.clearQuick();

		for (const MixerConnection* connection : track->getOutputConnections())
		{
			state->outputReceivers.add(connection->getOutputReceiver());
		}

		for (const TrackSendNode* send : track->getSends())
		{
			for (const MixerConnection* connection : send->getOutputConnections())
			{
				if (Track* receiverTrack = dynamic_cast<Track*>(connection->getOutputReceiver()))
				{
					state->outputReceivers.add(receiverTrack);
				}
			}
		}
	}
	
	for (const AudioInputProxy* proxy : inputProxies)
	{
		AudioCoreNodeProcessingState* const state = static_cast<AudioCoreNodeProcessingState*>(proxy->getProcessingState());

		state->outputReceivers.clearQuick();
		for (const MixerConnection* connection : proxy->getOutputConnections())
		{
			state->outputReceivers.add(connection->getOutputReceiver());
		}
	}

	for (const AudioOutputProxy* proxy : outputProxies)
	{
		AudioCoreNodeProcessingState* const state = static_cast<AudioCoreNodeProcessingState*>(proxy->getProcessingState());
		state->numInputNodes = proxy->getInputConnections().size();
	}

	AudioCoreNodeProcessingState* const soloBusState = static_cast<AudioCoreNodeProcessingState*>(soloBus ->getProcessingState());
	soloBusState->outputReceivers.clearQuick();
	for (const MixerConnection* connection : soloBus->getOutputConnections())
	{
		soloBusState->outputReceivers.add(connection->getOutputReceiver());
	}
}

bool AudioCoreProcessor::isAudioProcessorThread(Thread::ThreadID id) const
{
	if (id == nullptr)
	{
		jassertfalse;
		return false;
	}
	
	if (id == processThreadId)
		return true;

	for (Worker* worker : processWorkers)
	{
		if (worker->getThreadId() == id)
			return true;
	}

	return false;
}

AudioCoreProcessor::ProcessJob* AudioCoreProcessor::popProcessJob()
{
	if (!processingArguments.jobsCanBePoped)
		return nullptr;

	return jobsQueue.pop();
}

void AudioCoreProcessor::notifyAllWorkers() const
{
	if (processingShouldStop) return;
	
	for (Worker* worker : processWorkers)
		worker->notify();		
}

void AudioCoreProcessor::notifyReceiverNodeJobDone()
{
	if (processingShouldStop) return;

	jobProcessed();
}

void AudioCoreProcessor::notifyTrackJobDone(Track* track)
{
	if (processingShouldStop) return;

	if (soloBus->getSoloedTracks().size() != 0 && soloBus->getSoloedTracks().contains(track))
		soloedTrackProcessed();

	notifyNodeReceiverInputProcessed(track);
	jobProcessed();
}

void AudioCoreProcessor::notifyNodeJobDone(MixerSourceNode* node)
{
	if (processingShouldStop) return;

	notifyNodeReceiverInputProcessed(node);
	jobProcessed();
}

void AudioCoreProcessor::notifyNodeReceiverInputProcessed(MixerSourceNode* node)
{
	for (MixerReceiverNode* receiver : static_cast<AudioCoreNodeProcessingState*>(node->getProcessingState())->outputReceivers)
	{
		AudioCoreNodeProcessingState* const receiverState = static_cast<AudioCoreNodeProcessingState*>(receiver->getProcessingState());
		
		if (++receiverState->numInputNodesProcessed == receiverState->numInputNodes)
			nodeReady(receiver, true);
	}
}

void AudioCoreProcessor::nodeReady(MixerNode* node, bool notifyWorkers)
{
	jobsQueue.push(static_cast<AudioCoreNodeProcessingState*>(node->getProcessingState())->processJob);

	if (notifyWorkers)
	{
		notifyAllWorkers();
	}
}

void AudioCoreProcessor::jobProcessed()
{
#ifdef JUCE_DEBUG
	if (nodesToProcess < 1)
		DBG("Some node was processed twice");
#endif

	if (--nodesToProcess <= 0)
		workerSyncEvent.signal();
}

void AudioCoreProcessor::soloedTrackProcessed()
{
	AudioCoreNodeProcessingState* busState = static_cast<AudioCoreNodeProcessingState*>(soloBus->getProcessingState());
	if (++busState->numInputNodesProcessed == busState->numInputNodes)
		nodeReady(soloBus, true);
}

void AudioCoreProcessor::setSchemaDirty()
{
	schemaDirty = true;
	schemaLatencyDirty = true;
}

void AudioCoreProcessor::setSchemaLatencyDirty()
{
	schemaLatencyDirty = true;
}

/* ----------------------------------------------------------- */
/* ----------------------------------------------------------- */

AudioCoreProcessor::Worker::Worker(AudioCoreProcessor* p, int cpuCoreIndex)
	: processor(p), Thread("AudioCoreProcessorWorker " + String(++workerIndex))
{
	if (cpuCoreIndex >= 0)
		setAffinityMask(1 << cpuCoreIndex);
}

AudioCoreProcessor::Worker::~Worker()
{
	stopThread(100);
}

void AudioCoreProcessor::Worker::run()
{
	ProcessJob* job;
	int tryIndex = 0;

	while (!threadShouldExit())
	{
		job = processor->popProcessJob();

		if (job != nullptr)
		{
			job->process();
			tryIndex = 0;
		}
		else if (++tryIndex < 50)
		{
			Thread::yield();
		}
		else
		{
			tryIndex = 0;
			wait(1);
		}
	}
}

int AudioCoreProcessor::Worker::workerIndex = 0;

/* ----------------------------------------------------------- */
/* ----------------------------------------------------------- */

AudioCoreProcessor::TrackProcessJob::TrackProcessJob(AudioCoreProcessor* p, Track* t)
	: processor(p), track(t)
{
}

AudioCoreProcessor::TrackProcessJob::~TrackProcessJob()
{

}


void AudioCoreProcessor::TrackProcessJob::process()
{
	const int numSamples = processor->processingArguments.numSamples;
	const int numInputs = track->getInputConnections().size();
	processingBuffer.setSize(track->getNumInputChannels(), numSamples);

	TrackProcessingState* const processingState = static_cast<TrackProcessingState*>(track->getProcessingState());
	
	if (numInputs == 0)
	{
		processingBuffer.clear();
	}
	else if (numInputs == 1)
	{
		const MixerSourceNode* inputNode = track->getInputConnections().getFirst()->getInputSource();
		processingBuffer.copyFrom(0, inputNode->getAudioOutputBuffer(), -processingState->inputLatencySamples[0], numSamples);
	}
	else
	{
		processingBuffer.clear();
		int i = 0;
		for (const MixerConnection* connection : track->getInputConnections())
		{
			const MixerSourceNode* inputNode = connection->getInputSource();
			processingBuffer.addFrom(0, inputNode->getAudioOutputBuffer(), -processingState->inputLatencySamples[i], numSamples);
			++i;
		}
	}

	track->processBlock(processingBuffer);

	processingState->processed = true;
	processor->notifyTrackJobDone(track);
}

/* ----------------------------------------------------------- */
/* ----------------------------------------------------------- */

AudioCoreProcessor::InputProxyProcessJob::InputProxyProcessJob(AudioCoreProcessor* pc, AudioInputProxy* px)
	: processor(pc), inputProxy(px)
{
}

AudioCoreProcessor::InputProxyProcessJob::~InputProxyProcessJob()
{
}

void AudioCoreProcessor::InputProxyProcessJob::process()
{
	const ProcessingCallbackArguments& args = processor->processingArguments;
	inputProxy->gatherData(args.inputChannelData, args.numInputChannels, args.numSamples);

	static_cast<AudioCoreNodeProcessingState*>(inputProxy->getProcessingState())->processed = true;
	processor->notifyNodeJobDone(inputProxy);
}

/* ----------------------------------------------------------- */
/* ----------------------------------------------------------- */

AudioCoreProcessor::OutputProxyProcessJob::OutputProxyProcessJob(AudioCoreProcessor* pc, AudioOutputProxy* px)
	: processor(pc), outputProxy(px)
{
}

AudioCoreProcessor::OutputProxyProcessJob::~OutputProxyProcessJob()
{

}

void AudioCoreProcessor::OutputProxyProcessJob::process()
{
	const ProcessingCallbackArguments& args = processor->processingArguments;
	outputProxy->streamData(args.outputChannelData, args.numOutputChannels, args.numSamples);

	static_cast<AudioCoreNodeProcessingState*>(outputProxy->getProcessingState())->processed = true;
	processor->notifyReceiverNodeJobDone();
}

AudioCoreProcessor::SoloBusProcessJob::SoloBusProcessJob(AudioCoreProcessor* pc, SoloBus* b)
	: processor(pc), soloBus(b)
{
}

AudioCoreProcessor::SoloBusProcessJob::~SoloBusProcessJob()
{
}

void AudioCoreProcessor::SoloBusProcessJob::process()
{
	TrackProcessingState* const processingState = static_cast<TrackProcessingState*>(soloBus->getProcessingState());
	soloBus->gatherData(processor->processingArguments.numSamples);

	processingState->processed = true;
	processor->notifyNodeJobDone(soloBus);
}
