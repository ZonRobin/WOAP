#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_mixer_mediator_node.h"
#include "woap_audio_buffer_extended.h"
#include "woap_api_osc_message_receiver.h"
#include "woap_api_consumer.h"
#include "woap_dca_controller.h"
#include "woap_bootstrapper.h"
#include "woap_stateful.h"

#define ADVANCED_TRACK_BUFFER_DEBUG 0
#if ADVANCED_TRACK_BUFFER_DEBUG
	#define ASSERT_BROKEN_BUFFER_DATA(buffer) buffer.checkForBrokenData(true)
#else
	#define ASSERT_BROKEN_BUFFER_DATA(buffer) ignoreUnused(buffer)
#endif

namespace WOAP
{
	class ParameterFloat;
	class ParameterBool;
	class AudioProcessorPlayerDouble;
	class TrackSendNode;

	class Track : 
		public AudioPlaybackInfoListener, 
		public MidiInputCallback, 
		public MixerMediatorNode,
		public API::APIConsumer,
		public Stateful
	{
	protected:
		struct AddPluginCompletitionCallback : public AudioPluginFormat::InstantiationCompletionCallback
		{
			AddPluginCompletitionCallback(Track* track, int position)
				: owner(track), position(position)
			{}

			virtual ~AddPluginCompletitionCallback()
			{
				owner->removePluginCompletitionCallback(this);
			}

			void completionCallback(AudioPluginInstance* instance, const String& error) override
			{
				if (execute)
				{
					owner->addPluginInstance(instance, error, position);
				}
			}

			void trackWillBeRemoved()
			{
				execute = false;
			}

			Track* owner;
			const int position;
			bool execute = true;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddPluginCompletitionCallback)
		};

	public:
		enum MessageType
		{
			GetDetail,
			SetGain,
			SetPan,
			SetMute,
			SetSolo,
			SetRecordArm,
			SetName,
			SetColour,
			AddSendNode, 
			RemoveSendNode, 
			MoveSendNode,
			AddPlugin,
			RemovePlugin,
			MovePlugin,
			AddInfoListener,
			RemoveInfoListener,
			AddControlsListener,
			RemoveControlsListener,
			AddPluginsListener,
			RemovePluginsListener,
			AddSendsListener,
			RemoveSendsListener 
		};

		enum Permission
		{
			_BaseId = 1100,
			ViewTrack,
			ModifyTrack
		};
		
		enum AudioSourcePosition : int
		{
			PrePlugins = 0,
			PreFader = 1000,
			PostFader = 1001
		};
	
		class PluginsListener
		{
		public:
			virtual void pluginAdded(AudioProcessorPlayerDouble* plugin, int position) = 0;
			virtual void pluginRemoved(AudioProcessorPlayerDouble* plugin) = 0;
			virtual void pluginMoved(AudioProcessorPlayerDouble* plugin, int oldPosition, int newPosition) = 0;
			virtual void aboutToDelete(Track*) = 0;
		};

		class SendsListener
		{
		public:
			virtual void sendAdded(TrackSendNode* node, int position) = 0;
			virtual void sendRemoved(TrackSendNode* node) = 0;
			virtual void sendMoved(TrackSendNode* node, int oldPosition, int newPosition) = 0;
			virtual void aboutToDelete(Track*) = 0;
		};

		Track(int numInputChannels, const String& name, Colour colour);
		virtual ~Track();

		virtual int getType() const = 0;

		virtual int getLatencySamples() const override;

		ParameterFloat* getGainParameter() const { return gainParameter; }
		ParameterFloat* getPanParameter() const { return panParameter; }
		ParameterBool* getMuteParameter() const { return muteParameter; }
		ParameterBool* getSoloParameter() const { return soloParameter; }
		ParameterBool* getRecordArmParameter() const { return recordArmParameter; }

		const AudioBufferDouble& getSoloOutputBuffer() const { return soloOutputBuffer; }

		virtual void colourChanged() override;

		void playbackAboutToStart(double sampleRate, int bufferSizeSamples) override;
		void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override;
		void handlePartialSysexMessage(MidiInput* source, const uint8* messageData, int numBytesSoFar, double timestamp) override;

		void addPluginByName(const String& name, int position = Track::PluginPositionEnd);
		void addPluginByUid(int uuid, int position = Track::PluginPositionEnd);
		void addPlugin(int pluginTypeId, int position = Track::PluginPositionEnd);
		void addPlugin(const PluginDescription* pluginInfo, int position = Track::PluginPositionEnd);
		void addPluginSync(const PluginDescription* pluginInfo, int position = Track::PluginPositionEnd);
		void addPluginInstance(AudioPluginInstance* newPlugin, const String& errorMsg, int position = Track::PluginPositionEnd);
		void addPluginPlayer(AudioProcessorPlayerDouble* newPlayer, const String& errorMsg, int position = Track::PluginPositionEnd);

		void removePlugin(const Uuid& pluginId);
		void removePlugin(AudioProcessorPlayerDouble* plugin);
		void removeAllPlugins();
		void movePlugin(int oldPosition, int newPosition);
		void movePlugin(const Uuid& pluginId, int newPosition);
		void movePlugin(AudioProcessorPlayerDouble* plugin, int newPosition);
		void replacePlugin(AudioProcessorPlayerDouble* newPlugin, AudioProcessorPlayerDouble* oldPlugin);

		AudioProcessorPlayerDouble* getPlugin(const Uuid& pluginId) const;
		const OwnedArray<AudioProcessorPlayerDouble>& getPlugins() const { return plugins; }

		void addPluginsListener(PluginsListener* listener) { pluginsListeners.add(listener); }
		void removePluginsListener(PluginsListener* listener) { pluginsListeners.remove(listener); }

		void addSendsListener(SendsListener* listener) { sendsListeners.add(listener); }
		void removeSendsListener(SendsListener* listener) { sendsListeners.remove(listener); }

		virtual void processBlock(const AudioBufferDouble& inputBuffer) override;

		const Array<float>& getSignalLevels() const { return signalLevels; }
		bool areSignalLevelsInSoloMode() const { return signalLevelsInSoloMode; }

		virtual Array<API::AddressWithTypeId> getMessageTypesToListenTo() const override;

		virtual void messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId) override;

		void addPluginCompletitionCallback(AddPluginCompletitionCallback* callback);
		void removePluginCompletitionCallback(AddPluginCompletitionCallback* callback);

		int getLatencySamplesForAudioSource(int audioSourcePosition) const;
		virtual int getLatencySamplesInTree(bool includeOwnLatency = true) const override;
		void treeLatencyChanged();

		virtual bool hasOutputReceiverAssigned(const MixerReceiverNode* receiver, bool recursive = true) const override;

		const OwnedArray<TrackSendNode>& getSends() const { return sends; }
		TrackSendNode* getSend(const Uuid& sendId) const;
		int getNextSendNumber() const;

		TrackSendNode* addSend(int audioSourcePosition, int position = -1);
		void addSend(TrackSendNode* send, int position = -1);

		void removeSend(const Uuid& sendId);
		void removeSend(TrackSendNode* send);
		void removeAllSends();

		void moveSend(int oldPosition, int newPosition);
		void moveSend(const Uuid& sendId, int newPosition);
		void moveSend(TrackSendNode* send, int newPosition);
		
		DCAController& getDCAController() { return dcaController; }

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;
		static Track* fromState(const ValueTree& state);
		
	protected:
		ScopedPointer<ParameterFloat> gainParameter;
		ScopedPointer<ParameterFloat> panParameter;

		ScopedPointer<ParameterBool> muteParameter;
		ScopedPointer<ParameterBool> soloParameter;
		ScopedPointer<ParameterBool> recordArmParameter;
		DCAController dcaController;

		AudioBufferDouble ioCallbackInputBuffer;
		AudioBufferDouble soloOutputBuffer;

		OwnedArray<AudioProcessorPlayerDouble> plugins;
		Array<Track::AddPluginCompletitionCallback*> pluginCompletitionCallbacks;
		OwnedArray<TrackSendNode> sends;

		ListenerList<PluginsListener> pluginsListeners;
		ListenerList<SendsListener> sendsListeners;

		int treeLatencySamples = -1;
		Array<float> signalLevels = { 0.0f, 0.0f };
		bool signalLevelsInSoloMode = false;

		OwnedArray<LinearSmoothedValue<double>> channelGains;

		const static int PluginPositionEnd = -1;

	private:
		const static Bootstrapper::Initializer bootstrapInitializer;
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
	};

	/* ------ AUDIO TRACK -------------------------- */

	class AudioTrack : public Track
	{
	public:
		static const int Type = 1;
		AudioTrack(int numInputChannels = NumChannels::Stereo, const String& name = "Track");
		AudioTrack(const Uuid& idToUse, int numInputChannels = NumChannels::Stereo, const String& name = "Track");

		virtual int getType() const override { return Type; }
		virtual bool canConnectOutputReceiver(const MixerReceiverNode* receiver) const override;
		virtual bool canConnectInputSource(const MixerSourceNode* source) const override;
		virtual Colour getDefaultColour() override;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioTrack)
	};

	/* ------ FX TRACK -------------------------- */

	class FxTrack : public Track
	{
	public:
		static const int Type = 2;
		FxTrack(int numInputChannels = NumChannels::Stereo, const String& name = "Fx track");
		FxTrack(const Uuid& idToUse, int numInputChannels = NumChannels::Stereo, const String& name = "Fx track");

		virtual int getType() const override { return Type; }
		virtual bool canConnectOutputReceiver(const MixerReceiverNode* receiver) const override;
		virtual bool canConnectInputSource(const MixerSourceNode* source) const override;
		virtual Colour getDefaultColour() override;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FxTrack)
	};

	/* ------ BUS TRACK -------------------------- */

	class MixBusTrack : public Track
	{
	public:
		static const int Type = 3;
		MixBusTrack(int numInputChannels = NumChannels::Stereo, const String& name = "Mix bus");
		MixBusTrack(const Uuid& idToUse, int numInputChannels = NumChannels::Stereo, const String& name = "Mix bus");

		virtual int getType() const override { return Type; }
		virtual bool canConnectOutputReceiver(const MixerReceiverNode* receiver) const override;
		virtual bool canConnectInputSource(const MixerSourceNode* source) const override;
		virtual Colour getDefaultColour() override;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixBusTrack)
	};

	/* ------ OUTPUT BUS TRACK -------------------------- */

	class OutputBusTrack : public Track
	{
	public:
		static const int Type = 4;
		OutputBusTrack(int numInputChannels = NumChannels::Stereo, const String& name = "Master");
		OutputBusTrack(const Uuid& idToUse, int numInputChannels = NumChannels::Stereo, const String& name = "Master");

		virtual int getType() const override { return Type; }
		virtual bool canConnectOutputReceiver(const MixerReceiverNode* receiver) const override;
		virtual bool canConnectInputSource(const MixerSourceNode* source) const override;
		virtual Colour getDefaultColour() override;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutputBusTrack)
	};
}
