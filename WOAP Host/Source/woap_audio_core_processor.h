#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_audio_core.h"
#include "woap_track.h"
#include "woap_track_send_node.h"
#include "woap_fast_fifo.h"
#include <atomic>

using std::atomic;

namespace WOAP
{
	class AudioCoreProcessor : public AudioIODeviceCallback, public AudioCore::Listener, public Track::PluginsListener, public Track::SendsListener, public TrackSendNode::AudioSourcePositionListener
	{
	public:
		class ProcessJob;
		class AudioCoreNodeProcessingState;
	protected:
		class Worker;
		struct ProcessingCallbackArguments
		{
			const float** inputChannelData;
			int numInputChannels;
			float** outputChannelData;
			int numOutputChannels;
			int numSamples;
			bool jobsCanBePoped = false;

		private:
			JUCE_LEAK_DETECTOR(ProcessingCallbackArguments)
		};
	public:
		AudioCoreProcessor(AudioCore* core);
		virtual ~AudioCoreProcessor();

		void startProcessing();
		void stopProcessing();

		virtual void audioDeviceIOCallback(const float** inputChannelData,
			int numInputChannels,
			float** outputChannelData,
			int numOutputChannels,
			int numSamples) override;

		virtual void audioDeviceAboutToStart(AudioIODevice*) override;
		virtual void audioDeviceStopped() override;

		virtual void audioInputProxyAdded(AudioInputProxy*) override;
		virtual void audioOutputProxyAdded(AudioOutputProxy*) override;
		virtual void audioInputProxyRemoved(AudioInputProxy*) override;
		virtual void audioOutputProxyRemoved(AudioOutputProxy*) override;
		virtual void trackAdded(Track*, int position) override;
		virtual void trackRemoved(Track*) override;
		virtual void connectionsChanged() override;
		virtual void aboutToDelete(AudioCore*) override;

		virtual void pluginAdded(AudioProcessorPlayerDouble*, int) override;
		virtual void pluginRemoved(AudioProcessorPlayerDouble*) override;
		virtual void pluginMoved(AudioProcessorPlayerDouble*, int, int) override;
		virtual void aboutToDelete(Track*) override {}

		virtual void sendAudioSourcePositionChanged(TrackSendNode* send) override;
		virtual void aboutToDelete(TrackSendNode*) override {}

		virtual void sendAdded(TrackSendNode* node, int position) override;
		virtual void sendRemoved(TrackSendNode* node) override;
		virtual void sendMoved(TrackSendNode* node, int oldPosition, int newPosition) override;

		void fillDebugSamples(AudioBuffer<float>& bufferToFill, int numChannels, int numSamples) const;
		void recountGlobalTreeLatency() const;
		void recountTrackInputLatencies() const;
		void recountNodesSchemaCache() const;

		const CriticalSection& getProcessLock() const { return processLock; }
		bool isAudioProcessorThread(Thread::ThreadID) const;

		ProcessJob* popProcessJob();

		void notifyAllWorkers() const;
		void notifyReceiverNodeJobDone();
		void notifyTrackJobDone(Track* track);
		void notifyNodeJobDone(MixerSourceNode* node);
		void notifyNodeReceiverInputProcessed(MixerSourceNode* node);
		
	protected:
		void nodeReady(MixerNode* node, bool notifyWorkers);
		void jobProcessed();
		void soloedTrackProcessed();
		void setSchemaDirty();
		void setSchemaLatencyDirty();

		bool processingShouldStop = false;

		AudioCore* audioCore;

		OwnedArray<Worker> processWorkers;
		WaitableEvent workerSyncEvent;
		ProcessingCallbackArguments processingArguments;

		CriticalSection processLock;
		Thread::ThreadID processThreadId;

		Array<Track*> tracks;
		Array<AudioInputProxy*> inputProxies;
		Array<AudioOutputProxy*> outputProxies;
		Array<MixerNode*> mixerNodes;
		SoloBus* soloBus;

		FastFIFO<ProcessJob*> jobsQueue;
		bool schemaDirty = true;
		bool schemaLatencyDirty = true;

		double audioCallbackMaxTimeMs = 1;

		atomic<int> nodesToProcess = 0;
		Array<MixerNode*> nodesReadyBeforeProcessing;

		class Worker : public Thread
		{
		public:
			Worker(AudioCoreProcessor* processor, int cpuCoreIndex = -1);
			virtual ~Worker();

			virtual void run() override;

		protected:
			AudioCoreProcessor* processor;
			static int workerIndex;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Worker)
		};

	public:
		class ProcessJob
		{
		public:
			ProcessJob() {}
			virtual ~ProcessJob() {}
			virtual void process() = 0;
		};

		class TrackProcessJob : public ProcessJob
		{
		public:
			TrackProcessJob(AudioCoreProcessor* p, Track* t);
			virtual ~TrackProcessJob();
			virtual void process() override;

		protected:
			Track* track;
			AudioCoreProcessor* processor;
			AudioBufferDouble processingBuffer;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackProcessJob)
		};

		class InputProxyProcessJob : public ProcessJob
		{
		public:
			InputProxyProcessJob(AudioCoreProcessor* pc, AudioInputProxy* px);
			virtual ~InputProxyProcessJob();
			virtual void process() override;

		protected:
			AudioInputProxy* inputProxy;
			AudioCoreProcessor* processor;
		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InputProxyProcessJob)
		};

		class OutputProxyProcessJob : public ProcessJob
		{
		public:
			OutputProxyProcessJob(AudioCoreProcessor* pc, AudioOutputProxy* px);
			virtual ~OutputProxyProcessJob();
			virtual void process() override;

		protected:
			AudioOutputProxy* outputProxy;
			AudioCoreProcessor* processor;
		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutputProxyProcessJob)
		};

		class SoloBusProcessJob : public ProcessJob
		{
		public:
			SoloBusProcessJob(AudioCoreProcessor* pc, SoloBus* b);
			virtual ~SoloBusProcessJob();
			virtual void process() override;

		protected:
			SoloBus* soloBus;
			AudioCoreProcessor* processor;
		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloBusProcessJob)
		};

		class AudioCoreNodeProcessingState : public MixerNode::ProcessingState
		{
		public:
			AudioCoreNodeProcessingState() {}
			virtual ~AudioCoreNodeProcessingState() {}

			inline bool canBeProcessed() const noexcept { return !processed && numInputNodesProcessed == numInputNodes; }

			int numInputNodes = 0;
			atomic<int> numInputNodesProcessed = 0;
			bool processed = false;
			ScopedPointer<ProcessJob> processJob;
			Array<MixerReceiverNode*> outputReceivers;
			
		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreNodeProcessingState)
		};

		struct TrackProcessingState : public AudioCoreNodeProcessingState
		{
		public:
			Array<int> inputLatencySamples;
		};
		
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreProcessor)
	};
}
