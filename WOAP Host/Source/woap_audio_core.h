#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap.h"
#include "woap_singleton.h"
#include "woap_api_consumer.h"
#include "woap_stateful.h"
#include "woap_bootstrapper.h"

#define ASSERT_ONLY_AUDIO_PROCESS_THREADS jassert(AudioCore::getInstance()->isAudioProcessorThread(Thread::getCurrentThreadId()))

namespace WOAP
{
	class AudioCoreProcessor;
	class AudioInputProxy;
	class AudioOutputProxy;

	class Track;
	class TrackSendNode;
	class AudioTrack;
	class FxTrack;
	class MixBusTrack;
	class OutputBusTrack;
	class SoloBus;

	class MixerSourceNode;
	class MixerReceiverNode;
	class MixerNode;
	class MixerConnection;

	class AudioCore : public API::APIConsumer, public Stateful, public AudioIODeviceCallback
	{
	public:
		class Listener
		{
		public:
			Listener() {}
			virtual ~Listener() {}

			virtual void audioInputProxyAdded(AudioInputProxy*) {}
			virtual void audioOutputProxyAdded(AudioOutputProxy*) {}
			virtual void audioInputProxyRemoved(AudioInputProxy*) {}
			virtual void audioOutputProxyRemoved(AudioOutputProxy*) {}
			virtual void trackAdded(Track*, int /*position*/) {}
			virtual void trackRemoved(Track*) {}
			virtual void trackMoved(Track*, int /*oldPosition*/, int /*newPosition*/) {}
			virtual void connectionsChanged() {}
			virtual void globalAudioLatencyChanged(int /*newLatencySamples*/, float /*newLatencyMs*/) {}
			virtual void aboutToDelete(AudioCore*) = 0;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCore::Listener)
		};

		class SignalLevelListener
		{
		public:
			virtual void aboutToDelete(AudioCore*) = 0;
		};

		enum MessageType
		{
			AddTrack,
			RemoveTrack,
			MoveTrack,
			ConnectNodes,
			RemoveNodesConnection,
			GetTracksList,
			GetInputProxiesList,
			GetOutputProxiesList,
			GetAvailableInputSourcesList,
			GetAvailableOutputReceiversList,
			GetGlobalAudioLatency,
			AddListener,
			RemoveListener,
			AddSignalLevelListener,
			RemoveSignalLevelListener
		};

		enum Permission
		{
			_BaseId = 1000,
			ViewNodes,
			ModifyNodes
		};

		void shutdown();

		int getGlobalAudioLatencySamples() const { return globalAudioLatencySamples; }
		float getGlobalAudioLatencyMs() const;

		double getCurrentSampleRate() const { return currentSampleRate; }
		int getCurrentBufferSizeSamples() const { return currentBufferSizeSamples; }

		bool isPlaybackActive() const { return playbackActive; }

		void setDefaultMixerSetup();

		bool isPlaybackRunning() const { return audioDevicePlaybackRunning; }
		void initializeDevicePlayback();
		void stopDevicePlayback();
		void pauseDevicePlayback();
		void resumeDevicePlayback();

		void selectAudioDevice();
		void updateAudioProxiesForCurrentDevice();

		virtual void audioDeviceAboutToStart(AudioIODevice* device) override;
		virtual void audioDeviceIOCallback(const float**, int, float**, int, int) override { jassertfalse; }
		virtual void audioDeviceStopped() override;

		void addListener(AudioCore::Listener* listener) { listeners.add(listener); }
		void removeListener(AudioCore::Listener* listener) { listeners.remove(listener); }

		void addSignalLevelListener(AudioCore::SignalLevelListener* listener) { signalLevelListeners.add(listener); }
		void removeSignalLevelListener(AudioCore::SignalLevelListener* listener) { signalLevelListeners.remove(listener); }

		void addAudioPlaybackInfoListener(AudioPlaybackInfoListener* listener);
		void removeAudioPlaybackInfoListener(AudioPlaybackInfoListener* listener);

		Track* addTrack(int trackType, int position = -1);
		AudioTrack* addAudioTrack(int position = -1);
		FxTrack* addFxTrack(int position = -1);
		MixBusTrack* addMixBusTrack(int position = -1);
		OutputBusTrack* addOutputBusTrack(int position = -1);

		AudioInputProxy* addInputProxy(const String& name, int channelIndex, bool unique = false);
		AudioInputProxy* addInputProxy(const String& name, int leftChannelIndex, int rightChannelIndex, bool unique = false);
		void addInputProxy(AudioInputProxy* proxy);

		AudioOutputProxy* addOutputProxy(const String& name, int channelIndex, bool unique = false);
		AudioOutputProxy* addOutputProxy(const String& name, int leftChannelIndex, int rightChannelIndex, bool unique = false);
		void addOutputProxy(AudioOutputProxy* proxy);

		void removeInputProxy(AudioInputProxy* proxy);
		void removeOutputProxy(AudioOutputProxy* proxy);

		void createInputProxiesForActiveChannels(const BigInteger& channels);
		void createOutputProxiesForActiveChannels(const BigInteger& channels);

		void removeAllInputProxies();
		void removeInputProxiesWithInactiveChannel(const BigInteger& channels);

		void removeAllOutputProxies();
		void removeOutputProxiesWithInactiveChannel(const BigInteger& channels);

		Track* getTrack(const Uuid& trackId) const;
		MixerSourceNode* getSourceNode(const Uuid& nodeId) const;
		MixerReceiverNode* getReceiverNode(const Uuid& nodeId) const;
		MixerNode* getNode(const Uuid& nodeId) const;

		void removeAllNodes();
		int getNumNodes() const;

		void removeTrack(const Uuid& trackId);
		void removeTrack(Track* track);
		void removeAllTracks();

		void moveTrack(int oldPosition, int newPosition);
		void moveTrack(const Uuid& trackId, int newPosition);
		void moveTrack(Track* track, int newPosition);

		inline const OwnedArray<Track>& getTracks() const noexcept { return tracks; }
		inline const OwnedArray<AudioInputProxy>& getAudioInputProxies() const noexcept { return inputProxies; }
		inline const OwnedArray<AudioOutputProxy>& getAudioOutputProxies() const noexcept { return outputProxies; }

		AudioInputProxy* getAudioInputProxy(const Array<int>& channelIndexes) const;
		AudioOutputProxy* getAudioOutputProxy(const Array<int>& channelIndexes) const;

		int getNumTracks() const;
		int getNumTracks(int trackType) const;
		int getNextTrackNumber(int trackType) const;

		SoloBus* getSoloBus() const { return soloBus; }

		AudioDeviceManager* getAudioDeviceManager() const { return audioDeviceManager; }
		AudioCoreProcessor* getAudioCoreProcessor() const { return audioCoreProcessor; }

		String getNextTrackName(int trackType) const;

		Array<MixerSourceNode*> getAvailableInputSources(const Uuid& receiverId) const;
		Array<MixerSourceNode*> getAvailableInputSources(const MixerReceiverNode* receiver) const;

		Array<MixerReceiverNode*> getAvailableOutputReceivers(const Uuid& sourceId) const;
		Array<MixerReceiverNode*> getAvailableOutputReceivers(const MixerSourceNode* source) const;

		void recountGlobalTreeLatency();
		const CriticalSection& getProcessLock() const;
		bool isAudioProcessorThread(Thread::ThreadID id) const;

		bool connect(const Uuid& sourceId, const Uuid& receiverId);
		bool connect(MixerSourceNode* source, MixerReceiverNode* receiver);
		bool isConnected(MixerSourceNode* source, MixerReceiverNode* receiver) const;

		const OwnedArray<MixerConnection>& getConnections() const { return connections; }

		void removeConnection(const Uuid& sourceId, const Uuid& receiverId, bool notifyListeners = true);
		void removeConnection(MixerSourceNode* source, MixerReceiverNode* receiver, bool notifyListeners = true);
		void removeAllConnections(Track* track, bool notifyListeners = true);
		void removeAllConnections(TrackSendNode* send, bool notifyListeners = true);
		void removeAllConnections(MixerSourceNode* node, bool notifyListeners = true);
		void removeAllConnections(MixerReceiverNode* node, bool notifyListeners = true);

		virtual Array<API::AddressWithTypeId> getMessageTypesToListenTo() const override;
		virtual void messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId) override;

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;

		void saveAudioDeviceManagerSettings();

	protected:
		void addTrack(Track* track, int position = -1);

		ScopedPointer<AudioCoreProcessor> audioCoreProcessor;
		ScopedPointer<AudioDeviceManager> audioDeviceManager;

		bool audioDevicePlaybackRunning = false;

		OwnedArray<Track> tracks;
		OwnedArray<AudioInputProxy> inputProxies;
		OwnedArray<AudioOutputProxy> outputProxies;
		OwnedArray<MixerConnection> connections;
		ScopedPointer<SoloBus> soloBus;

		ListenerList<AudioCore::Listener> listeners;
		ListenerList<AudioCore::SignalLevelListener> signalLevelListeners;
		ListenerList<AudioPlaybackInfoListener> audioPlaybackInfoListeners;

		int globalAudioLatencySamples = 0;
		BigInteger lastActiveInputChannels;
		BigInteger lastActiveOutputChannels;

		double currentSampleRate = 44100;
		int currentBufferSizeSamples = 64;
		bool playbackActive = false;

	private:
		const static Bootstrapper::Initializer bootstrapInitializer;
		SINGLETON(AudioCore)
	};
}

