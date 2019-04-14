#include "woap_audio_processor_player_double.h"
#include "woap_api_messages_plugin.h"
#include "woap_api_osc_remote_client.h"
#include "woap_parameter_bool.h"
#include "woap_api_listener_bridges_plugin_listener_bridge.h"
#include "woap_plugin_manager.h"
#include "woap_audio_core.h"

using namespace WOAP;
using namespace WOAP::API::Messages;
using namespace WOAP::API::ListenerBridges;

AudioProcessorPlayerDouble::AudioProcessorPlayerDouble(Track* tr, AudioPluginInstance* processorToPlay, bool doDoublePrecisionProcessing)
	: track(tr), isDoublePrecision(doDoublePrecisionProcessing), lastPluginEditorPosition(-1, -1)
{
	bypassParameter = new ParameterBool("bypassed", "Bypassed", false);
	activeStateParameter = new ParameterBool("active", "Active", true);

	AudioCore::getInstance()->addAudioPlaybackInfoListener(this);

	setProcessor(processorToPlay);
	name = processorToPlay->getName();

	initializeAPIConsumer();
}

AudioProcessorPlayerDouble::AudioProcessorPlayerDouble(const Uuid& idToUse, Track* tr, AudioPluginInstance* processorToPlay, bool doDoublePrecisionProcessing)
	: Identifiable(idToUse), track(tr), isDoublePrecision(doDoublePrecisionProcessing), lastPluginEditorPosition(-1, -1)
{
	bypassParameter = new ParameterBool("bypassed", "Bypassed", false);
	activeStateParameter = new ParameterBool("active", "Active", true);

	AudioCore::getInstance()->addAudioPlaybackInfoListener(this);

	setProcessor(processorToPlay);
	name = processorToPlay->getName();

	initializeAPIConsumer();
}

AudioProcessorPlayerDouble::~AudioProcessorPlayerDouble()
{
	listeners.call(&Listener::aboutToDelete, this);
	parameterListeners.call(&ParameterListener::aboutToDelete, this);

	terminateAPIConsumer();

	AudioCore::getInstance()->removeAudioPlaybackInfoListener(this);

	ScopedPointer<AudioProcessor> proc(isPrepared ? nullptr : processor);
	setProcessor(nullptr);
}

void AudioProcessorPlayerDouble::setProcessor(AudioPluginInstance* processorToPlay)
{
	if (processor == processorToPlay)
		return;

	if (processorToPlay != nullptr && sampleRate > 0 && blockSize > 0)
	{
		processorToPlay->setRateAndBufferSizeDetails(sampleRate, blockSize);
		processorToPlay->disableNonMainBuses();

		if (processorToPlay->getBusCount(true) > 0)
		{
			if (processorToPlay->setChannelLayoutOfBus(true, 0, AudioChannelSet::canonicalChannelSet(NumChannels::Stereo)))
				numMainBusInputChannels = NumChannels::Stereo;
			else if (processorToPlay->setChannelLayoutOfBus(true, 0, AudioChannelSet::canonicalChannelSet(NumChannels::Mono)))
				numMainBusInputChannels = NumChannels::Mono;
			else if (processorToPlay->setChannelLayoutOfBus(true, 0, AudioChannelSet::canonicalChannelSet(NumChannels::None)))
				numMainBusInputChannels = NumChannels::None;
			else
				jassert("Unknown bus configuration");
		}
		else
		{
			numMainBusInputChannels = NumChannels::None;
		}

		if (processorToPlay->getBusCount(false) > 0)
		{
			if (processorToPlay->setChannelLayoutOfBus(false, 0, AudioChannelSet::canonicalChannelSet(NumChannels::Stereo)))
				numMainBusOutputChannels = NumChannels::Stereo;
			else if (processorToPlay->setChannelLayoutOfBus(false, 0, AudioChannelSet::canonicalChannelSet(NumChannels::Mono)))
				numMainBusOutputChannels = NumChannels::Mono;
			else if (processorToPlay->setChannelLayoutOfBus(false, 0, AudioChannelSet::canonicalChannelSet(NumChannels::None)))
				numMainBusOutputChannels = NumChannels::None;
			else
				jassert("Unknown bus configuration");
		}
		else
		{
			numMainBusOutputChannels = NumChannels::None;
		}

		numTotalInputChannels = processorToPlay->getTotalNumInputChannels();
		numTotalOutputChannels = processorToPlay->getTotalNumOutputChannels();

		tempBuffer.setSize(jmax(numTotalInputChannels, numTotalOutputChannels), blockSize);

		const bool supportsDouble = processorToPlay->supportsDoublePrecisionProcessing() && isDoublePrecision;
		AudioProcessor::ProcessingPrecision precision = supportsDouble ? AudioProcessor::doublePrecision
			: AudioProcessor::singlePrecision;

		processorToPlay->addListener(this);
		processorToPlay->setProcessingPrecision(precision);
		processorToPlay->prepareToPlay(sampleRate, blockSize);
	}

	ScopedPointer<AudioProcessor> oldOne;

	{
		const ScopedLock sl(lock);
		oldOne = isPrepared ? processor : nullptr;
		processor = processorToPlay;
		isPrepared = true;
	}

	if (oldOne != nullptr) 
	{
		oldOne->removeListener(this);
		oldOne->releaseResources();
	}
}

AudioProcessorEditor* AudioProcessorPlayerDouble::createEditor() const
{
	return processor->createEditor();
}

void AudioProcessorPlayerDouble::setName(const String& newName)
{
	if (name == newName)
		return;

	name = newName;
	listeners.call(&Listener::nameChanged, this);	
}

String AudioProcessorPlayerDouble::getType() const
{
	if (processor == nullptr)
		return String();

	return processor->getPluginDescription().name;
}

int AudioProcessorPlayerDouble::getTypeId() const
{
	if (processor == nullptr)
		return -1;

	return processor->getPluginDescription().uid;
}

void AudioProcessorPlayerDouble::setDoublePrecisionProcessing(bool doublePrecision)
{
	if (doublePrecision == isDoublePrecision)
		return;
    
    const ScopedLock sl (lock);

    if (processor != nullptr)
    {
        processor->releaseResources();

        const bool supportsDouble = processor->supportsDoublePrecisionProcessing() && doublePrecision;
        AudioProcessor::ProcessingPrecision precision = supportsDouble ? AudioProcessor::doublePrecision
                                                                        : AudioProcessor::singlePrecision;

        processor->setProcessingPrecision (precision);
        processor->prepareToPlay (sampleRate, blockSize);
    }

    isDoublePrecision = doublePrecision;
}

void AudioProcessorPlayerDouble::processBlock(const AudioBufferDouble& inputBuffer, AudioBufferDouble& outputBuffer)
{
	ASSERT_ONLY_AUDIO_PROCESS_THREADS;

	/* these should have been prepared by audioDeviceAboutToStart()... */
	jassert(sampleRate > 0 && blockSize > 0);
	
	const ScopedLock sl(lock);
	
	const int numSamples = inputBuffer.getNumCurrentSamples();
	const int numBufferInputChannels = inputBuffer.getNumChannels();
	const int numBufferOutputChannels = outputBuffer.getNumChannels();
	const bool bypassed = bypassParameter->get();
	const bool notActive = processor->isSuspended() || !activeStateParameter->get();
	const int latencySamples = processor->getLatencySamples();
	
	incomingMidi.clear();
	messageCollector.removeNextBlockOfMessages(incomingMidi, numSamples);
	
	bypassDelayBuffer.setSize(numBufferInputChannels, numSamples, latencySamples);

	if (bypassed || notActive)
		bypassDelayBuffer.makeCopyOf(inputBuffer);
	else
		bypassDelayBuffer.pushToHistory(inputBuffer);

	if (notActive)
	{
		fillBypassSamples(outputBuffer);
		bypassDelayBuffer.shiftSamplesToHistory();
		return;
	}

	tempBuffer.setSize(jmax(numTotalInputChannels, numTotalOutputChannels), blockSize);
	switch (numMainBusInputChannels)
	{
	case NumChannels::Stereo:
		tempBuffer.copyFrom(0, 0, inputBuffer.getReadPointer(0), numSamples);
		tempBuffer.copyFrom(1, 0, inputBuffer.getReadPointer(1), numSamples);
		break;
	case NumChannels::Mono:
		tempBuffer.clearChannel(0);
		tempBuffer.addFrom(0, 0, inputBuffer.getReadPointer(0), numSamples);
		tempBuffer.addFrom(0, 0, inputBuffer.getReadPointer(1), numSamples);
		break;
	}

	for (int channelIndex = numMainBusInputChannels; channelIndex < numTotalInputChannels; ++channelIndex)
		tempBuffer.clearChannel(channelIndex);
	
	if (!processor->isUsingDoublePrecision())
	{
		const ScopedLock sl2(processor->getCallbackLock());
		conversionBuffer.makeCopyOf(tempBuffer);
		processor->processBlock(conversionBuffer.toJuceBuffer(), incomingMidi);

		if (!bypassed)
			tempBuffer.makeCopyOf(conversionBuffer);
	}
	else
	{
		const ScopedLock sl2(processor->getCallbackLock());
		processor->processBlock(tempBuffer.toJuceBuffer(), incomingMidi);
	}
		
	if (bypassed)
	{
		fillBypassSamples(outputBuffer);
		bypassDelayBuffer.shiftSamplesToHistory();
		return;
	}

	switch (numMainBusOutputChannels)
	{
	case NumChannels::Stereo:
		outputBuffer.copyFrom(0, 0, tempBuffer.getReadPointer(0), numSamples);
		outputBuffer.copyFrom(1, 0, tempBuffer.getReadPointer(1), numSamples);
		break;
	case NumChannels::Mono:
		outputBuffer.copyFrom(0, 0, tempBuffer.getReadPointer(0), numSamples);
		outputBuffer.copyFrom(1, 0, tempBuffer.getReadPointer(0), numSamples);
		break;
	default:
		outputBuffer.clearChannel(0);
		outputBuffer.clearChannel(1);
		break;
	}
}

void AudioProcessorPlayerDouble::fillBypassSamples(AudioBufferDouble& outputBuffer)
{
	const int numSamples = bypassDelayBuffer.getNumCurrentSamples();
	const int latencySamples = processor->getLatencySamples();

	switch (numMainBusOutputChannels)
	{
	case NumChannels::Stereo:
		outputBuffer.copyFrom(0, 0, bypassDelayBuffer.getReadPointer(0, -latencySamples), numSamples);
		outputBuffer.copyFrom(1, 0, bypassDelayBuffer.getReadPointer(1, -latencySamples), numSamples);
		break;
	case NumChannels::Mono:
		outputBuffer.copyFrom(0, 0, bypassDelayBuffer.getReadPointer(0, -latencySamples), numSamples);
		outputBuffer.copyFrom(1, 0, bypassDelayBuffer.getReadPointer(0, -latencySamples), numSamples);
		break;
	default:
		outputBuffer.clearChannel(0);
		outputBuffer.clearChannel(1);
		break;
	}
}

void AudioProcessorPlayerDouble::playbackAboutToStart(double newSampleRate, int newBufferSizeSamples)
{
	playbackAboutToStop();

	const ScopedLock sl(lock);

	sampleRate = newSampleRate;
	blockSize = newBufferSizeSamples;

	messageCollector.reset(sampleRate);

	if (processor == nullptr)
		return;

	if (isPrepared)
		processor->releaseResources();

	AudioPluginInstance* const oldProcessor = processor;
	setProcessor(nullptr);
	setProcessor(oldProcessor);
}

void AudioProcessorPlayerDouble::playbackAboutToStop()
{
	const ScopedLock sl(lock);

	if (processor != nullptr && isPrepared)
		processor->releaseResources();

	sampleRate = 0.0;
	blockSize = 0;
	isPrepared = false;
	tempBuffer.setSize(1, 1);
}

void AudioProcessorPlayerDouble::handleIncomingMidiMessage(MidiInput*, const MidiMessage& message)
{
	messageCollector.addMessageToQueue(message);
}

Array<API::AddressWithTypeId> AudioProcessorPlayerDouble::getMessageTypesToListenTo() const
{
	const String idStr = getId().toDashedString();
	return Array<API::AddressWithTypeId>({
		API::AddressWithTypeId(PluginGetDetail::getAddress(idStr), MessageType::GetDetail),
		API::AddressWithTypeId(PluginGetParameterValues::getAddress(idStr), MessageType::GetParameterValues),
		API::AddressWithTypeId(PluginGetParameters::getAddress(idStr), MessageType::GetParameters),
		API::AddressWithTypeId(PluginSetName::getAddress(idStr), MessageType::SetName),
		API::AddressWithTypeId(PluginSetActive::getAddress(idStr), MessageType::SetActive),
		API::AddressWithTypeId(PluginSetBypass::getAddress(idStr), MessageType::SetBypass),
		API::AddressWithTypeId(PluginSetParameterValue::getAddress(idStr), MessageType::SetParameterValue),
		API::AddressWithTypeId(PluginAddListener::getAddress(idStr), MessageType::AddListener),
		API::AddressWithTypeId(PluginRemoveListener::getAddress(idStr), MessageType::RemoveListener),
		API::AddressWithTypeId(PluginAddParameterListener::getAddress(idStr), MessageType::AddParameterListener),
		API::AddressWithTypeId(PluginRemoveParameterListener::getAddress(idStr), MessageType::RemoveParameterListener),
		API::AddressWithTypeId(PluginAddBypassListener::getAddress(idStr), MessageType::AddBypassListener),
		API::AddressWithTypeId(PluginRemoveBypassListener::getAddress(idStr), MessageType::RemoveBypassListener),
		API::AddressWithTypeId(PluginAddActiveListener::getAddress(idStr), MessageType::AddActiveListener),
		API::AddressWithTypeId(PluginRemoveActiveListener::getAddress(idStr), MessageType::RemoveActiveListener)
	});
}

void AudioProcessorPlayerDouble::messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId)
{
	switch (messageTypeId)
	{
	case MessageType::GetDetail:
	{
		PluginGetDetail msg(message, this);

		if (!msg.isValid())
			return;
		
		client.sendOSCMessage(
			PluginDetail(
				msg, 
				this, 
				getName(), 
				getType(), 
				getTypeId(),
				bypassParameter->get(), 
				activeStateParameter->get()
			)
		);
		break;
	}
	case MessageType::GetParameterValues:
	{
		PluginGetParameterValues msg(message, this);

		if (!msg.isValid())
			return;

		const int numParams = processor->getNumParameters();

		var values;
		values.resize(numParams);

		for (int i = 0; i < numParams; ++i)
		{
			var val(processor->getParameter(i));
			val.append(var(processor->getParameterText(i)));
			values[i] = val;
		}

		client.sendOSCMessage(PluginParameterValues(msg, this, values));
		break;
	}
	case MessageType::GetParameters:
	{
		PluginGetParameters msg(message, this);

		if (!msg.isValid())
			return;

		client.sendOSCMessage(PluginParameters(msg, this, getParametersInfo()));
		break;
	}
	case MessageType::SetName:
	{
		PluginSetName msg(message, this);

		if (!msg.isValid())
			return;

		setName(msg.getNewName());
		break;
	}
	case MessageType::SetActive:
	{
		PluginSetActive msg(message, this);

		if (!msg.isValid())
			return;

		*activeStateParameter = msg.getNewState();
		break;
	}
	case MessageType::SetBypass:
	{
		PluginSetBypass msg(message, this);

		if (!msg.isValid())
			return;

		*bypassParameter = msg.getNewState();
		break;
	}
	case MessageType::SetParameterValue:
	{
		PluginSetParameterValue msg(message, this);

		if (!msg.isValid())
			return;

		setParameterNotifyingHost(msg.getParameterIndex(), msg.getNewValue());
		break;
	}
	case MessageType::AddListener:
	{
		PluginAddListener msg(message, this);

		if (!msg.isValid())
			return;

		PluginListenerBridge::addToList<Listener>(listeners, new PluginListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveListener:
	{
		PluginRemoveListener msg(message, this);

		if (!msg.isValid())
			return;

		PluginListenerBridge::removeFromList(listeners, *this, client);
		break;
	}
	case MessageType::AddParameterListener:
	{
		PluginAddParameterListener msg(message, this);

		if (!msg.isValid())
			return;

		PluginParameterListenerBridge::addToList<ParameterListener>(parameterListeners, new PluginParameterListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveParameterListener:
	{
		PluginRemoveParameterListener msg(message, this);

		if (!msg.isValid())
			return;

		PluginParameterListenerBridge::removeFromList(parameterListeners, *this, client);
		break;
	}
	case MessageType::AddBypassListener:
	{
		PluginAddBypassListener msg(message, this);

		if (!msg.isValid())
			return;

		PluginBypassListenerBridge::addToList<WOAP::Parameter::Listener>(bypassParameter->getListeners(), new PluginBypassListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveBypassListener:
	{
		PluginRemoveBypassListener msg(message, this);

		if (!msg.isValid())
			return;

		PluginBypassListenerBridge::removeFromList(bypassParameter->getListeners(), *this, client);
		break;
	}
	case MessageType::AddActiveListener:
	{
		PluginAddActiveListener msg(message, this);

		if (!msg.isValid())
			return;

		PluginActiveListenerBridge::addToList<WOAP::Parameter::Listener>(activeStateParameter->getListeners(), new PluginActiveListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveActiveListener:
	{
		PluginRemoveActiveListener msg(message, this);

		if (!msg.isValid())
			return;

		PluginActiveListenerBridge::removeFromList(activeStateParameter->getListeners(), *this, client);
		break;
	}
	default:
		handleUnknownMessage(getName(), client, message, messageTypeId);
		break;
	}
}

void AudioProcessorPlayerDouble::setParameter(int parameterIndex, float newValue)
{
	if (processor == nullptr)
		return;

	processor->setParameter(parameterIndex, newValue);
}

void AudioProcessorPlayerDouble::setParameterNotifyingHost(int parameterIndex, float newValue)
{
	if (processor == nullptr)
		return;

	processor->setParameterNotifyingHost(parameterIndex, newValue);
}

void AudioProcessorPlayerDouble::audioProcessorParameterChanged(AudioProcessor*, int parameterIndex, float newValue)
{
	parameterListeners.call(&ParameterListener::audioProcessorParameterChanged, this, parameterIndex, newValue);
}

void AudioProcessorPlayerDouble::audioProcessorParameterChangeGestureBegin(AudioProcessor*, int parameterIndex)
{
	parameterListeners.call(&ParameterListener::audioProcessorParameterChangeGestureBegin, this, parameterIndex);
}

void AudioProcessorPlayerDouble::audioProcessorParameterChangeGestureEnd(AudioProcessor*, int parameterIndex)
{
	parameterListeners.call(&ParameterListener::audioProcessorParameterChangeGestureEnd, this, parameterIndex);
}

Array<AudioProcessorPlayerDouble::ParameterInfo> AudioProcessorPlayerDouble::getParametersInfo() const
{
	const int numParams = processor->getNumParameters();

	Array<AudioProcessorPlayerDouble::ParameterInfo> params;
	
	for (int i = 0; i < numParams; ++i)
	{
		params.add(AudioProcessorPlayerDouble::ParameterInfo(
			i, 
			processor->getParameterName(i),
			processor->getParameter(i),
			processor->getParameterText(i)
		));
	}

	return params;
}

Array<float> AudioProcessorPlayerDouble::getParameterValues() const
{
	const int numParams = processor->getNumParameters();

	Array<float> values;
	values.ensureStorageAllocated(numParams);

	for (int i = 0; i < numParams; ++i)
	{
		values.add(processor->getParameter(i));
	}

	return values;
}

ValueTree AudioProcessorPlayerDouble::getState() const
{
	ValueTree state("PLUGIN");

	StateHelper::setIdProperty(state, this);
	state.setProperty("name", getName(), nullptr);
	state.setProperty("pluginTypeId", processor->getPluginDescription().uid, nullptr);
	state.setProperty("bypass", var(*bypassParameter), nullptr);
	state.setProperty("active", var(*activeStateParameter), nullptr);
	state.setProperty("pluginEditorPositionX", var(lastPluginEditorPosition.x), nullptr);
	state.setProperty("pluginEditorPositionY", var(lastPluginEditorPosition.y), nullptr);

	ValueTree parameterValues("PARAMETERS");
	for (float value : getParameterValues())
	{
		parameterValues.addChild(ValueTree("PARAMETER").setProperty("value", var(value), nullptr), -1, nullptr);
	}
	state.addChild(parameterValues, -1, nullptr);

	return state;
}

void AudioProcessorPlayerDouble::setState(const ValueTree& state)
{
	if (!state.hasType("PLUGIN"))
		return;

	setName(StateHelper::getStringProperty(state, "name"));
	*bypassParameter = StateHelper::getBoolProperty(state, "bypass");
	*activeStateParameter = StateHelper::getBoolProperty(state, "active");
	lastPluginEditorPosition.setX(StateHelper::getIntProperty(state, "pluginEditorPositionX", -1));
	lastPluginEditorPosition.setY(StateHelper::getIntProperty(state, "pluginEditorPositionY", -1));

	int paramIndex = 0;
	for (ValueTree param : state.getChildWithName("PARAMETERS"))
	{
		if (!param.hasType("PARAMETER"))
			continue;

		setParameter(paramIndex++, StateHelper::getFloatProperty(param, "value"));
	}
}

AudioProcessorPlayerDouble* AudioProcessorPlayerDouble::fromState(Track* track, const ValueTree& state, String& errorMsg)
{
	if (!state.hasType("PLUGIN"))
		return nullptr;

	const Uuid stateId = StateHelper::getIdProperty(state);
	if (stateId.isNull())
		return nullptr;

	const int pluginTypeId = StateHelper::getIntProperty(state, "pluginTypeId", -1);
	const PluginDescription* pluginDesc = PluginManager::getInstance()->getPluginType(pluginTypeId);
	if (pluginDesc == nullptr)
		return nullptr;

	AudioPluginInstance* pluginInstance = PluginManager::getInstance()->getAudioPluginFormatManager()->createPluginInstance(
		*pluginDesc, 
		AudioCore::getInstance()->getCurrentSampleRate(),
		AudioCore::getInstance()->getCurrentBufferSizeSamples(),
		errorMsg
	);

	if (pluginInstance == nullptr || errorMsg.isNotEmpty())
		return nullptr;

	AudioProcessorPlayerDouble* player = new AudioProcessorPlayerDouble(stateId, track, pluginInstance);
	player->setState(state);

	return player;
}
