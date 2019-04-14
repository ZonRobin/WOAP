#include "woap_track.h"
#include "woap_audio_core.h"
#include "woap_look_and_feel.h"
#include "woap_plugin_manager.h"
#include "woap_parameter_float.h"
#include "woap_parameter_bool.h"
#include "woap_audio_processor_player_double.h"
#include "woap_audio_input_proxy.h"
#include "woap_audio_output_proxy.h"
#include "woap_api_listener_bridges_track_listener_bridge.h"
#include "woap_api_osc_remote_client.h"
#include "woap_api_messages_track.h"
#include "woap_mixer_receiver_node.h"
#include "woap_mixer_connection.h"
#include "woap_user_permission.h"
#include "woap_track_send_node.h"

using namespace WOAP;
using namespace API::Messages;
using namespace API::ListenerBridges;

Track::Track(int numInputChannels, const String& name, Colour colour)
	: dcaController(this)
{
	setName(name);
	setColour(colour);
	setNumInputChannels(numInputChannels);
	setNumOutputChannels(NumChannels::Stereo);

	panParameter = new ParameterFloat("pan", "Pan", -1.0f, 1.0f, 0.0f);
	gainParameter = new ParameterFloat("gain", "Gain", -145.0f, 10.0f, 0.0f);

	muteParameter = new ParameterBool("mute", "Mute", false);
	soloParameter = new ParameterBool("solo", "Solo", false);
	recordArmParameter = new ParameterBool("record_armed", "Record armed", false);

	channelGains.add(new LinearSmoothedValue<double>());
	channelGains.add(new LinearSmoothedValue<double>());

	AudioCore::getInstance()->addAudioPlaybackInfoListener(this);

	initializeAPIConsumer();
}

Track::~Track()
{
	terminateAPIConsumer();

	AudioCore::getInstance()->removeAudioPlaybackInfoListener(this);

	for (AddPluginCompletitionCallback* callback : pluginCompletitionCallbacks)
	{
		callback->trackWillBeRemoved();
	}

	pluginsListeners.call(&Track::PluginsListener::aboutToDelete, this);
	sendsListeners.call(&Track::SendsListener::aboutToDelete, this);
}

const Bootstrapper::Initializer Track::bootstrapInitializer([](Bootstrapper& bootstrapper) {
	bootstrapper.reportPermissions({
		new UserPermission(Permission::ViewTrack, "View track"),
		new UserPermission(Permission::ModifyTrack, "Modify track")
	});
});

int Track::getLatencySamples() const
{
	if (plugins.isEmpty())
	{
		return 0;
	}

	int latencySum = 0;

	for (const AudioProcessorPlayerDouble* plugin : plugins)
	{
		latencySum += plugin->getCurrentProcessor()->getLatencySamples();
	}

	return latencySum;
}

void Track::addPluginByName(const String& pluginName, int position)
{
	const KnownPluginList& list = PluginManager::getInstance()->getKnownPluginList();

	for (PluginDescription* desc : list)
	{
		if (desc->name == pluginName)
		{
			addPlugin(desc, position);
			return;
		}
	}
}

void Track::addPluginByUid(int uid, int position)
{
	const KnownPluginList& list = PluginManager::getInstance()->getKnownPluginList();

	for (PluginDescription* desc : list)
	{
		if (desc->uid == uid)
		{
			addPlugin(desc, position);
			return;
		}
	}
}

void Track::addPlugin(int pluginTypeId, int position)
{
	const KnownPluginList& list = PluginManager::getInstance()->getKnownPluginList();

	for (PluginDescription* desc : list)
	{
		if (desc->uid == pluginTypeId)
		{
			addPlugin(desc, position);
			return;
		}
	}
}

void Track::addPlugin(const PluginDescription* pluginDesc, int position)
{
	if (pluginDesc == nullptr)
		return;
	
	AddPluginCompletitionCallback* callback = new AddPluginCompletitionCallback(this, position);
	addPluginCompletitionCallback(callback);
		
	PluginManager::getInstance()->getAudioPluginFormatManager()->createPluginInstanceAsync(
		*pluginDesc,
		AudioCore::getInstance()->getCurrentSampleRate(),
		AudioCore::getInstance()->getCurrentBufferSizeSamples(),
		callback
	);
}

void Track::addPluginSync(const PluginDescription* pluginDesc, int position)
{
	String errorMsg;

	AudioPluginInstance* plugin = PluginManager::getInstance()->getAudioPluginFormatManager()->createPluginInstance(
		*pluginDesc,
		AudioCore::getInstance()->getCurrentSampleRate(),
		AudioCore::getInstance()->getCurrentBufferSizeSamples(),
		errorMsg
	);

	addPluginInstance(plugin, errorMsg, position);
}

void Track::addPluginInstance(AudioPluginInstance* newPlugin, const String& errorMsg, int position)
{	
	if (newPlugin == nullptr || errorMsg.isNotEmpty())
	{
		AlertWindow::showMessageBox(
			AlertWindow::WarningIcon,
			TRANS("Couldn't create plugin instance"),
			errorMsg
		);

		return;
	}

	newPlugin->enableAllBuses();
	AudioProcessorPlayerDouble* player = new AudioProcessorPlayerDouble(this, newPlugin);

	MessageManager::callAsync([this, player, position]() {
		const ScopedLock sl(getProcessLock());
		ASSERT_ONLY_MESSAGE_THREAD;

		plugins.insert(position, player);

		pluginsListeners.call(&PluginsListener::pluginAdded, player, position);
	});
}

void Track::addPluginPlayer(AudioProcessorPlayerDouble* newPlayer, const String& errorMsg, int position)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (newPlayer->getCurrentProcessor() == nullptr || errorMsg.isNotEmpty())
	{
		AlertWindow::showMessageBox(
			AlertWindow::WarningIcon,
			TRANS("Couldn't create plugin instance"),
			errorMsg
		);

		return;
	}

	newPlayer->getCurrentProcessor()->enableAllBuses();

	const ScopedLock sl(getProcessLock());
	plugins.insert(position, newPlayer);

	pluginsListeners.call(&PluginsListener::pluginAdded, newPlayer, position);
}

void Track::removePlugin(const Uuid& pluginId)
{
	removePlugin(getPlugin(pluginId));
}

void Track::removePlugin(AudioProcessorPlayerDouble* plugin)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (plugin == nullptr) 
	{
		jassertfalse;
		return;
	}

	const int index = plugins.indexOf(plugin);
	if (index == -1)
	{
		jassertfalse;
		return;
	}

	const ScopedPointer<AudioProcessorPlayerDouble> toDelete(plugin);

	const ScopedLock sl(getProcessLock());
	plugins.remove(index, false);

	for (TrackSendNode* send : sends)
	{
		const int sendSourcePos = send->getAudioSourcePosition();
		if (sendSourcePos > index && sendSourcePos < AudioSourcePosition::PreFader)
			send->setAudioSourcePosition(sendSourcePos - 1);
	}

	pluginsListeners.call(&PluginsListener::pluginRemoved, plugin);
}

void Track::removeAllPlugins()
{
	ASSERT_ONLY_MESSAGE_THREAD;
	const ScopedLock sl(getProcessLock());

	for (int i = plugins.size() - 1; i >= 0; --i)
	{
		ScopedPointer<AudioProcessorPlayerDouble> plugin(plugins.removeAndReturn(i));
		pluginsListeners.call(&PluginsListener::pluginRemoved, plugin);
	}
}

void Track::movePlugin(int oldPosition, int newPosition)
{
	if (oldPosition >= plugins.size())
	{
		jassertfalse;
		return;
	}

	movePlugin(plugins[oldPosition], newPosition);
}

void Track::movePlugin(const Uuid& pluginId, int newPosition)
{
	movePlugin(getPlugin(pluginId), newPosition);
}

void Track::movePlugin(AudioProcessorPlayerDouble* plugin, int newPosition)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (plugin == nullptr) 
	{
		jassertfalse;
		return;
	}

	const int oldPosition = plugins.indexOf(plugin);
	if (oldPosition == -1)
	{
		jassertfalse;
		return;
	}

	const ScopedLock sl(getProcessLock());
	plugins.move(oldPosition, newPosition);

	pluginsListeners.call(&PluginsListener::pluginMoved, plugin, oldPosition, newPosition);
}

void Track::replacePlugin(AudioProcessorPlayerDouble* newPlugin, AudioProcessorPlayerDouble* oldPlugin)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	const int position = plugins.indexOf(oldPlugin);
	if (position == -1)
	{
		jassertfalse;
		return;
	}

	const ScopedPointer<AudioProcessorPlayerDouble> toDelete(oldPlugin);

	const ScopedLock sl(getProcessLock());
	plugins.set(position, newPlugin, false);
	
	pluginsListeners.call(&PluginsListener::pluginRemoved, oldPlugin);
	pluginsListeners.call(&PluginsListener::pluginAdded, newPlugin, position);
}

void Track::colourChanged()
{
	const Colour clr = getColour();

	for (TrackSendNode* send : sends)
		send->setColour(clr);
}

void Track::playbackAboutToStart(double sampleRate, int)
{
	channelGains[0]->reset(sampleRate, GainSmoothingTimeSecs);
	channelGains[1]->reset(sampleRate, GainSmoothingTimeSecs);

	audioOutputBuffer.clearWithHistory();
}

void Track::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message)
{
	for (AudioProcessorPlayerDouble* plugin : plugins)
	{
		plugin->handleIncomingMidiMessage(source, message);
	}
}

void Track::handlePartialSysexMessage(MidiInput* source, const uint8* messageData, int numBytesSoFar, double timestamp)
{
	for (AudioProcessorPlayerDouble* plugin : plugins)
	{
		plugin->handlePartialSysexMessage(source, messageData, numBytesSoFar, timestamp);
	}
}

AudioProcessorPlayerDouble* Track::getPlugin(const Uuid& pluginId) const
{
	for (AudioProcessorPlayerDouble* plugin : plugins)
	{
		if (plugin->getId() == pluginId)
			return plugin;
	}

	return nullptr;
}

void Track::processBlock(const AudioBufferDouble& inputBuffer)
{
	ASSERT_ONLY_AUDIO_PROCESS_THREADS;

	const int numOutputChannels = getNumOutputChannels();
	const int numSamples = inputBuffer.getNumCurrentSamples();
	const bool soloed = *soloParameter;

	if (treeLatencySamples < 0)
		treeLatencySamples = getLatencySamplesInTree();

	audioOutputBuffer.setSize(numOutputChannels, numSamples, treeLatencySamples);
	audioOutputBuffer.shiftSamplesToHistory();

	if (soloed)
		soloOutputBuffer.setSize(numOutputChannels, numSamples);

	ASSERT_BROKEN_BUFFER_DATA(audioOutputBuffer);

	if (getType() == AudioTrack::Type)
		clipCache.testBuffer(inputBuffer);

	for (TrackSendNode* send : sends)
	{
		if (send->getAudioSourcePosition() == AudioSourcePosition::PrePlugins)
			send->processBlock(inputBuffer);
	}
	
	if (!plugins.isEmpty())
	{
		ioCallbackInputBuffer.setSize(numOutputChannels, numSamples);
		ioCallbackInputBuffer.copyFrom(0, inputBuffer, 0, numSamples);

		const int lastPluginIndex = plugins.size() - 1;
		int pluginIndex = 0;
		for (AudioProcessorPlayerDouble* plugin : plugins)
		{
			plugin->processBlock(ioCallbackInputBuffer, audioOutputBuffer);
			ASSERT_BROKEN_BUFFER_DATA(audioOutputBuffer);

			const int audioSourcePositionIndex = pluginIndex + 1;
			for (TrackSendNode* send : sends)
			{
				if (send->getAudioSourcePosition() == audioSourcePositionIndex)
					send->processBlock(audioOutputBuffer);
			}

			if (pluginIndex == lastPluginIndex)
				break;

			ioCallbackInputBuffer.makeCopyOf(audioOutputBuffer);
			++pluginIndex;
		}
	}
	else
	{
		audioOutputBuffer.copyFrom(0, inputBuffer, 0, numSamples);
	}
	
	for (TrackSendNode* send : sends)
	{
		if (send->getAudioSourcePosition() == AudioSourcePosition::PreFader)
			send->processBlock(audioOutputBuffer);
	}

	if (soloed)
		soloOutputBuffer.makeCopyOf(audioOutputBuffer);
	else
		soloOutputBuffer.clear();

	double leftChannelGain = 0;
	double rightChannelGain = 0;

	if (!*muteParameter)
	{
		leftChannelGain = Decibels::decibelsToGain<float>(*gainParameter, -145.0f);
		rightChannelGain = Decibels::decibelsToGain<float>(*gainParameter, -145.0f);

		if (*panParameter > 0)
			leftChannelGain = leftChannelGain - *panParameter * leftChannelGain;
		else
			rightChannelGain = rightChannelGain + *panParameter * rightChannelGain;
	}

	channelGains[0]->setValue(leftChannelGain);
	channelGains[1]->setValue(rightChannelGain);

	channelGains[0]->applyGain(audioOutputBuffer.getWritePointer(0), numSamples);
	channelGains[1]->applyGain(audioOutputBuffer.getWritePointer(1), numSamples);

	ASSERT_BROKEN_BUFFER_DATA(audioOutputBuffer);

	for (TrackSendNode* send : sends)
	{
		if (send->getAudioSourcePosition() == AudioSourcePosition::PostFader)
			send->processBlock(audioOutputBuffer);
	}

	if (soloed)
	{
		signalLevelsInSoloMode = true;
		signalLevels.setUnchecked(0, (float)soloOutputBuffer.getMagnitude(0, 0, numSamples));
		signalLevels.setUnchecked(1, (float)soloOutputBuffer.getMagnitude(1, 0, numSamples));
	}
	else
	{
		signalLevelsInSoloMode = false;
		signalLevels.setUnchecked(0, (float)audioOutputBuffer.getMagnitude(0, 0, numSamples));
		signalLevels.setUnchecked(1, (float)audioOutputBuffer.getMagnitude(1, 0, numSamples));
	}

	if (getType() == OutputBusTrack::Type)
		clipCache.testBuffer(audioOutputBuffer);		
}

Array<API::AddressWithTypeId> Track::getMessageTypesToListenTo() const
{
	const String idStr = getId().toDashedString();

	return Array<API::AddressWithTypeId>({
		API::AddressWithTypeId(TrackGetDetail::getAddress(idStr), MessageType::GetDetail),
		API::AddressWithTypeId(TrackSetGain::getAddress(idStr), MessageType::SetGain),
		API::AddressWithTypeId(TrackSetPan::getAddress(idStr), MessageType::SetPan),
		API::AddressWithTypeId(TrackSetSolo::getAddress(idStr), MessageType::SetSolo),
		API::AddressWithTypeId(TrackSetMute::getAddress(idStr), MessageType::SetMute),
		API::AddressWithTypeId(TrackSetRecordArm::getAddress(idStr), MessageType::SetRecordArm),
		API::AddressWithTypeId(TrackSetName::getAddress(idStr), MessageType::SetName),
		API::AddressWithTypeId(TrackSetColour::getAddress(idStr), MessageType::SetColour),
		API::AddressWithTypeId(TrackAddSendNode::getAddress(idStr), MessageType::AddSendNode),
		API::AddressWithTypeId(TrackRemoveSendNode::getAddress(idStr), MessageType::RemoveSendNode),
		API::AddressWithTypeId(TrackMoveSendNode::getAddress(idStr), MessageType::MoveSendNode),
		API::AddressWithTypeId(TrackAddPlugin::getAddress(idStr), MessageType::AddPlugin),
		API::AddressWithTypeId(TrackRemovePlugin::getAddress(idStr), MessageType::RemovePlugin),
		API::AddressWithTypeId(TrackMovePlugin::getAddress(idStr), MessageType::MovePlugin),
		API::AddressWithTypeId(TrackAddInfoListener::getAddress(idStr), MessageType::AddInfoListener),
		API::AddressWithTypeId(TrackRemoveInfoListener::getAddress(idStr), MessageType::RemoveInfoListener),
		API::AddressWithTypeId(TrackAddControlsListener::getAddress(idStr), MessageType::AddControlsListener),
		API::AddressWithTypeId(TrackRemoveControlsListener::getAddress(idStr), MessageType::RemoveControlsListener),
		API::AddressWithTypeId(TrackAddPluginsListener::getAddress(idStr), MessageType::AddPluginsListener),
		API::AddressWithTypeId(TrackRemovePluginsListener::getAddress(idStr), MessageType::RemovePluginsListener),
		API::AddressWithTypeId(TrackAddSendsListener::getAddress(idStr), MessageType::AddSendsListener),
		API::AddressWithTypeId(TrackRemoveSendsListener::getAddress(idStr), MessageType::RemoveSendsListener)
	});
}

void Track::messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId)
{
	switch (messageTypeId)
	{
	case MessageType::GetDetail:
	{
		TrackGetDetail msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ViewTrack))
			return;

		StringArray pluginIds;
		for (AudioProcessorPlayerDouble* plugin : plugins)
		{
			pluginIds.add(plugin->getId().toDashedString());
		}

		StringArray sendIds;
		for (TrackSendNode* send : sends)
		{
			sendIds.add(send->getId().toDashedString());
		}

		client.sendOSCMessage(
			TrackDetail(
				msg,
				this,
				getType(),
				*gainParameter, 
				*panParameter,
				*muteParameter, 
				*soloParameter,
				*recordArmParameter,
				getName(),
				getColour(),
				pluginIds,
				sendIds
		));
		break;
	}
	case MessageType::SetGain:
	{
		TrackSetGain msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		*gainParameter = msg.getNewValue();
		break;
	}
	case MessageType::SetPan:
	{
		TrackSetPan msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		*panParameter = msg.getNewValue();
		break;
	}
	case MessageType::SetSolo:
	{
		TrackSetSolo msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		*soloParameter = msg.getNewState();
		break;
	}
	case MessageType::SetMute:
	{
		TrackSetMute msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		*muteParameter = msg.getNewState();
		break;
	}
	case MessageType::SetRecordArm:
	{
		TrackSetRecordArm msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		*recordArmParameter = msg.getNewState();
		break;
	}
	case MessageType::SetName:
	{
		TrackSetName msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		setName(msg.getNewName());
		break;
	}
	case MessageType::SetColour:
	{
		TrackSetColour msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		setColour(msg.getNewColour());
		break;
	}
	case MessageType::AddSendNode:
	{
		TrackAddSendNode msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		addSend(msg.getAudioSourcePosition(), msg.getSendPosition());
		break;
	}
	case MessageType::RemoveSendNode:
	{
		TrackRemoveSendNode msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		removeSend(msg.getSendId());
		break;
	}
	case MessageType::MoveSendNode:
	{
		TrackMoveSendNode msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		moveSend(msg.getSendId(), msg.getNewPosition());
		break;
	}
	case MessageType::AddPlugin:
	{
		TrackAddPlugin msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		addPlugin(msg.getPluginTypeId(), msg.getPluginPosition());
		break;
	}
	case MessageType::RemovePlugin:
	{
		TrackRemovePlugin msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		removePlugin(msg.getPluginId());
		break;
	}
	case MessageType::MovePlugin:
	{
		TrackMovePlugin msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ModifyTrack))
			return;

		movePlugin(msg.getPluginId(), msg.getNewPosition());
		break;
	}
	case MessageType::AddInfoListener:
	{
		TrackAddInfoListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ViewTrack))
			return;

		TrackInfoListenerBridge::addToList<Track::Listener>(listeners, new TrackInfoListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveInfoListener:
	{
		TrackRemoveInfoListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ViewTrack))
			return;

		TrackInfoListenerBridge::removeFromList(listeners, *this, client);
		break;
	}
	case MessageType::AddControlsListener:
	{
		TrackAddControlsListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ViewTrack))
			return;

		TrackGainListenerBridge::addToList<WOAP::Parameter::Listener>(gainParameter->getListeners(), new TrackGainListenerBridge(*this, client));
		TrackPanListenerBridge::addToList<WOAP::Parameter::Listener>(panParameter->getListeners(), new TrackPanListenerBridge(*this, client));
		TrackMuteListenerBridge::addToList<WOAP::Parameter::Listener>(muteParameter->getListeners(), new TrackMuteListenerBridge(*this, client));
		TrackSoloListenerBridge::addToList<WOAP::Parameter::Listener>(soloParameter->getListeners(), new TrackSoloListenerBridge(*this, client));
		TrackRecordArmListenerBridge::addToList<WOAP::Parameter::Listener>(recordArmParameter->getListeners(), new TrackRecordArmListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveControlsListener:
	{
		TrackRemoveControlsListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ViewTrack))
			return;
		
		TrackGainListenerBridge::removeFromList(gainParameter->getListeners(), *this, client);
		TrackPanListenerBridge::removeFromList(panParameter->getListeners(), *this, client);
		TrackMuteListenerBridge::removeFromList(muteParameter->getListeners(), *this, client);
		TrackSoloListenerBridge::removeFromList(soloParameter->getListeners(), *this, client);
		TrackRecordArmListenerBridge::removeFromList(recordArmParameter->getListeners(), *this, client);
		break;
	}
	case MessageType::AddPluginsListener:
	{
		TrackAddPluginsListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ViewTrack))
			return;

		TrackPluginsListenerBridge::addToList<PluginsListener>(pluginsListeners, new TrackPluginsListenerBridge(*this, client));
		break;
	}
	case MessageType::RemovePluginsListener:
	{
		TrackRemovePluginsListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ViewTrack))
			return;

		TrackPluginsListenerBridge::removeFromList(pluginsListeners, *this, client);
		break;
	}
	case MessageType::AddSendsListener:
	{
		TrackAddSendsListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ViewTrack))
			return;

		TrackSendsListenerBridge::addToList<SendsListener>(sendsListeners, new TrackSendsListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveSendsListener:
	{
		TrackRemoveSendsListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Permission::ViewTrack))
			return;

		TrackSendsListenerBridge::removeFromList(sendsListeners, *this, client);
		break;
	}
	default:
		handleUnknownMessage(getName(), client, message, messageTypeId);		
		break;
	}
}

void Track::addPluginCompletitionCallback(AddPluginCompletitionCallback* callback)
{
	pluginCompletitionCallbacks.add(callback);
}

void Track::removePluginCompletitionCallback(AddPluginCompletitionCallback* callback)
{
	pluginCompletitionCallbacks.removeAllInstancesOf(callback);
}

int Track::getLatencySamplesForAudioSource(int audioSourcePosition) const
{
	if (audioSourcePosition <= 0)
		return 0;

	audioSourcePosition = jmin(plugins.size(), audioSourcePosition);

	int latency = 0;

	for (int i = 0; i < audioSourcePosition; ++i)
	{
		latency += plugins.getUnchecked(i)->getCurrentProcessor()->getLatencySamples();
	}

	return latency;
}

int Track::getLatencySamplesInTree(bool includeOwnLatency) const
{
	int ownLatencySamples = 0;
	if (!sends.isEmpty() || includeOwnLatency)
		ownLatencySamples = getLatencySamples();

	int maxLatency = 0;

	for (const MixerConnection* connection : outputConnections)
	{
		if (includeOwnLatency)
			maxLatency = jmax(maxLatency, connection->getOutputReceiver()->getLatencySamplesInTree() + ownLatencySamples);
		else
			maxLatency = jmax(maxLatency, connection->getOutputReceiver()->getLatencySamplesInTree());
	}

	for (TrackSendNode* send : sends)
	{
		for (const MixerConnection* connection : send->getOutputConnections())
		{
			if (send->getAudioSourcePosition() == AudioSourcePosition::PostFader)
			{
				if (includeOwnLatency)
					maxLatency = jmax(maxLatency, connection->getOutputReceiver()->getLatencySamplesInTree() + ownLatencySamples);
				else
					maxLatency = jmax(maxLatency, connection->getOutputReceiver()->getLatencySamplesInTree());
			}
			else
			{
				if (includeOwnLatency)
					maxLatency = jmax(maxLatency, connection->getOutputReceiver()->getLatencySamplesInTree() + getLatencySamplesForAudioSource(send->getAudioSourcePosition()));
				else
					maxLatency = jmax(maxLatency, connection->getOutputReceiver()->getLatencySamplesInTree() - ownLatencySamples + getLatencySamplesForAudioSource(send->getAudioSourcePosition()));
			}
		}
	}

	return maxLatency;
}

void Track::treeLatencyChanged()
{
	ASSERT_ONLY_AUDIO_PROCESS_THREADS;

	treeLatencySamples = -1;
	for (TrackSendNode* send : sends)
	{
		send->treeLatencyChanged();
	}
}


bool Track::hasOutputReceiverAssigned(const MixerReceiverNode* receiver, bool recursive) const
{
	if (!recursive)
	{
		for (const MixerConnection* connection : outputConnections)
			if (connection->getOutputReceiver() == receiver)
				return true;

		return false;
	}

	for (const MixerConnection* connection : outputConnections)
	{
		if (connection->getOutputReceiver() == receiver)
			return true;

		if (const MixerSourceNode* source = dynamic_cast<const MixerSourceNode*>(connection->getOutputReceiver()))
			if (source->hasOutputReceiverAssigned(receiver, recursive))
				return true;
	}

	for (const TrackSendNode* send : sends)
	{
		for (const MixerConnection* connection : send->getOutputConnections())
		{
			if (connection->getOutputReceiver() == receiver)
				return true;

			if (const MixerSourceNode* source = dynamic_cast<const MixerSourceNode*>(connection->getOutputReceiver()))
				if (source->hasOutputReceiverAssigned(receiver, recursive))
					return true;
		}
	}

	return false;
}

TrackSendNode* Track::getSend(const Uuid& sendId) const
{
	for (TrackSendNode* send : sends)
	{
		if (send->getId() == sendId)
			return send;
	}

	return nullptr;
}

int Track::getNextSendNumber() const
{
	Array<int> sendNumbers;
	for (TrackSendNode* send : sends)
	{
		sendNumbers.addUsingDefaultSort(getIntFromStringEnd(send->getName()));
	}

	return getFirstMissingNumber(sendNumbers, 1);
}

TrackSendNode* Track::addSend(int audioSourcePosition, int position)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	const int nextSendNumber = getNextSendNumber();
	
	TrackSendNode* send = new TrackSendNode(*this, audioSourcePosition);
	send->setName(String("Send ") + String(nextSendNumber));

	addSend(send, position);
	return send;
}

void Track::addSend(TrackSendNode* send, int position)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (&send->getOwnerTrack() != this) 
	{
		jassertfalse;
		return;
	}

	send->setColour(getColour());
	const ScopedLock sl(getProcessLock());
	sends.insert(position, send);

	sendsListeners.call(&SendsListener::sendAdded, send, position);
}

void Track::removeSend(const Uuid& sendId)
{
	removeSend(getSend(sendId));
}

void Track::removeSend(TrackSendNode* node)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	if (node == nullptr) 
	{
		jassertfalse;
		return;
	}

	const int index = sends.indexOf(node);
	if (index == -1)
	{
		jassertfalse;
		return;
	}

	const ScopedPointer<TrackSendNode> toDelete(node);

	AudioCore::getInstance()->removeAllConnections(node);

	const ScopedLock sl(getProcessLock());
	sends.remove(index, false);
		
	sendsListeners.call(&SendsListener::sendRemoved, node);
}

void Track::removeAllSends()
{
	ASSERT_ONLY_MESSAGE_THREAD;
	const ScopedLock sl(getProcessLock());

	for (int i = sends.size() - 1; i >= 0; --i)
	{
		ScopedPointer<TrackSendNode> send(sends.removeAndReturn(i));
		AudioCore::getInstance()->removeAllConnections(send);

		sendsListeners.call(&SendsListener::sendRemoved, send);
	}
}

void Track::moveSend(int oldPosition, int newPosition)
{
	if (oldPosition >= sends.size())
	{
		jassertfalse;
		return;
	}

	moveSend(sends[oldPosition], newPosition);
}

void Track::moveSend(const Uuid& sendId, int newPosition)
{
	moveSend(getSend(sendId), newPosition);
}

void Track::moveSend(TrackSendNode* send, int newPosition)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (send == nullptr) 
	{
		jassertfalse;
		return;
	}

	const int oldPosition = sends.indexOf(send);
	if (oldPosition == -1)
	{
		jassertfalse;
		return;
	}

	const ScopedLock sl(getProcessLock());
	sends.move(oldPosition, newPosition);

	sendsListeners.call(&SendsListener::sendMoved, send, oldPosition, newPosition);
}

ValueTree Track::getState() const
{
	ValueTree state("TRACK");

	StateHelper::setIdProperty(state, this);
	state.setProperty("type", getType(), nullptr);
	state.setProperty("name", getName(), nullptr);
	state.setProperty("colour", getColour().toDisplayString(false), nullptr);
	state.setProperty("numInputChannels", getNumInputChannels(), nullptr);

	state.setProperty("gain", var(*gainParameter), nullptr);
	state.setProperty("pan", var(*panParameter), nullptr);

	state.setProperty("mute", var(*muteParameter), nullptr);
	state.setProperty("solo", var(*soloParameter), nullptr);
	state.setProperty("recordArm", var(*recordArmParameter), nullptr);

	ValueTree pluginsState("PLUGINS");
	for (AudioProcessorPlayerDouble* plugin : plugins)
	{
		pluginsState.addChild(plugin->getState(), -1, nullptr);
	}
	state.addChild(pluginsState, -1, nullptr);

	ValueTree sendsState("SENDS");
	for (TrackSendNode* send : sends)
	{
		sendsState.addChild(send->getState(), -1, nullptr);
	}
	state.addChild(sendsState, -1, nullptr);

	return state;
}

void Track::setState(const ValueTree& state)
{
	if (!state.hasType("TRACK"))
		return;

	setName(StateHelper::getStringProperty(state, "name"));
	setColour(Colour::fromString("FF" + StateHelper::getStringProperty(state, "colour", "000000")));
	setNumInputChannels(StateHelper::getIntProperty(state, "numInputChannels"));
	
	*gainParameter = StateHelper::getFloatProperty(state, "gain", gainParameter->getDefaultValue());
	*panParameter = StateHelper::getFloatProperty(state, "pan", panParameter->getDefaultValue());

	*muteParameter = StateHelper::getBoolProperty(state, "mute", muteParameter->getDefaultValue());
	*soloParameter = StateHelper::getBoolProperty(state, "solo", soloParameter->getDefaultValue());
	*recordArmParameter = StateHelper::getBoolProperty(state, "recordArm", recordArmParameter->getDefaultValue());
	
	removeAllPlugins();
	for (ValueTree pluginState : state.getChildWithName("PLUGINS"))
	{
		String errorMsg;
		AudioProcessorPlayerDouble* pluginPlayer = AudioProcessorPlayerDouble::fromState(this, pluginState, errorMsg);

		if (pluginPlayer != nullptr)
			addPluginPlayer(pluginPlayer, errorMsg);
	}

	removeAllSends();
	for (ValueTree sendState : state.getChildWithName("SENDS"))
	{
		TrackSendNode* send = TrackSendNode::fromState(*this, sendState);
		if (send != nullptr)
			addSend(send);
	}
}

Track* Track::fromState(const ValueTree& state)
{
	if (!state.hasType("TRACK"))
		return nullptr;

	const Uuid trackId = StateHelper::getIdProperty(state);

	Track* tr;
	const int typeVal = StateHelper::getIntProperty(state, "type", -1);
	switch (typeVal)
	{
	case AudioTrack::Type:
		tr = new ::AudioTrack(trackId);
		break;
	case FxTrack::Type:
		tr = new ::FxTrack(trackId);
		break;
	case MixBusTrack::Type:
		tr = new ::MixBusTrack(trackId);
		break;
	case OutputBusTrack::Type:
		tr = new ::OutputBusTrack(trackId);
		break;
	default:
		return nullptr;
	}

	tr->setState(state);
	return tr;
}


/* ------ AUDIO TRACK -------------------------- */

AudioTrack::AudioTrack(int numInputChannels, const String& name)
	: Track(numInputChannels, name, getDefaultColour())
{

}

AudioTrack::AudioTrack(const Uuid& idToUse, int numInputChannels, const String& name)
	: MixerNode(idToUse), Track(numInputChannels, name, getDefaultColour())
{
}

bool AudioTrack::canConnectOutputReceiver(const MixerReceiverNode* receiver) const
{
	if (this == receiver)
		return false;

	if (const MixerSourceNode* source = dynamic_cast<const MixerSourceNode*>(receiver))
		if (source->hasOutputReceiverAssigned(this))
			return false;

	return nullptr != dynamic_cast<const Track*>(receiver)
		&& nullptr == dynamic_cast<const AudioOutputProxy*>(receiver)
		&& nullptr == dynamic_cast<const AudioTrack*>(receiver);
}

bool AudioTrack::canConnectInputSource(const MixerSourceNode* source) const
{
	if (this == source)
		return false;

	return nullptr != dynamic_cast<const AudioInputProxy*>(source);
}

Colour AudioTrack::getDefaultColour()
{
	return CustomColours::audioTrackColour;
}


/* ------ FX TRACK -------------------------- */

FxTrack::FxTrack(int numInputChannels, const String& name)
	: Track(numInputChannels, name, getDefaultColour())
{

}

FxTrack::FxTrack(const Uuid& idToUse, int numInputChannels, const String& name)
	: MixerNode(idToUse), Track(numInputChannels, name, getDefaultColour())
{
}

bool FxTrack::canConnectOutputReceiver(const MixerReceiverNode* receiver) const
{
	if (this == receiver)
		return false;

	if (const MixerSourceNode* source = dynamic_cast<const MixerSourceNode*>(receiver))
		if (source->hasOutputReceiverAssigned(this))
			return false;

	return nullptr != dynamic_cast<const Track*>(receiver)
		&& nullptr == dynamic_cast<const AudioTrack*>(receiver);
}

bool FxTrack::canConnectInputSource(const MixerSourceNode* source) const
{
	if (this == source)
		return false;

	if (const MixerReceiverNode* receiver = dynamic_cast<const MixerReceiverNode*>(source))
	{
		if (receiver->hasInputSourceAssigned(this))
			return false;

		for (TrackSendNode* send : sends)
			if (receiver->hasInputSourceAssigned(send))
				return false;
	}

	for (TrackSendNode* send : sends)
		if (send == source)
			return false;

	return (nullptr != dynamic_cast<const Track*>(source) || nullptr != dynamic_cast<const TrackSendNode*>(source))
		&& nullptr == dynamic_cast<const OutputBusTrack*>(source);
}


Colour FxTrack::getDefaultColour()
{
	return CustomColours::fxTrackColour;
}


/* ------ BUS TRACK -------------------------- */

MixBusTrack::MixBusTrack(int numInputChannels, const String& name)
	: Track(numInputChannels, name, getDefaultColour())
{
	
}

MixBusTrack::MixBusTrack(const Uuid& idToUse, int numInputChannels, const String& name)
	: MixerNode(idToUse), Track(numInputChannels, name, getDefaultColour())
{
}

bool MixBusTrack::canConnectOutputReceiver(const MixerReceiverNode* receiver) const
{
	if (this == receiver)
		return false;

	if (const MixerSourceNode* source = dynamic_cast<const MixerSourceNode*>(receiver))
		if (source->hasOutputReceiverAssigned(this))
			return false;

	return nullptr != dynamic_cast<const Track*>(receiver)
		&& nullptr == dynamic_cast<const AudioTrack*>(receiver);
}

bool MixBusTrack::canConnectInputSource(const MixerSourceNode* source) const
{
	if (this == source)
		return false;

	if (const MixerReceiverNode* receiver = dynamic_cast<const MixerReceiverNode*>(source))
	{
		if (receiver->hasInputSourceAssigned(this))
			return false;

		for (TrackSendNode* send : sends)
			if (receiver->hasInputSourceAssigned(send))
				return false;
	}

	for (TrackSendNode* send : sends)
		if (send == source)
			return false;

	return (nullptr != dynamic_cast<const Track*>(source) || nullptr != dynamic_cast<const TrackSendNode*>(source))
		&& nullptr == dynamic_cast<const OutputBusTrack*>(source);
}


Colour MixBusTrack::getDefaultColour()
{
	return CustomColours::mixBusTrackColour;
}


/* ------ OUTPUT BUS TRACK -------------------------- */

OutputBusTrack::OutputBusTrack(int numInputChannels, const String& name)
	: Track(numInputChannels, name, getDefaultColour())
{

}

OutputBusTrack::OutputBusTrack(const Uuid& idToUse, int numInputChannels, const String& name)
	: MixerNode(idToUse), Track(numInputChannels, name, getDefaultColour())
{
}

bool OutputBusTrack::canConnectOutputReceiver(const MixerReceiverNode* receiver) const
{
	if (this == receiver)
		return false;

	return nullptr != dynamic_cast<const AudioOutputProxy*>(receiver);
}

bool OutputBusTrack::canConnectInputSource(const MixerSourceNode* source) const
{
	if (this == source)
		return false;

	if (const MixerReceiverNode* receiver = dynamic_cast<const MixerReceiverNode*>(source))
		if (receiver->hasInputSourceAssigned(this))
			return false;

	return (nullptr != dynamic_cast<const Track*>(source)
		&& nullptr == dynamic_cast<const OutputBusTrack*>(source))
		|| nullptr != dynamic_cast<const TrackSendNode*>(source);
}

Colour OutputBusTrack::getDefaultColour()
{
	return CustomColours::outputBusTrackColour;
}