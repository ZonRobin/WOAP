#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap.h"
#include "woap_identifiable.h"
#include "woap_audio_clip_cache.h"

namespace WOAP
{
	class MixerNode : public Identifiable
	{
	public:
		class ProcessingState
		{
		public:
			ProcessingState() {}
			virtual ~ProcessingState() {}
		};

		class Listener
		{
		public:
			virtual void nameChanged(MixerNode* node) = 0;
			virtual void colourChanged(MixerNode* node) = 0;
			virtual void aboutToDelete(MixerNode* node) = 0;
		};

		MixerNode();
		MixerNode(const Uuid idToUse);
		virtual ~MixerNode();

		void addListener(Listener* listener) { listeners.add(listener); }
		void removeListener(Listener* listener) { listeners.remove(listener); }

		uint32 getLastClipTimeMilliseconds() const noexcept { return clipCache.getLastClipTimeMilliseconds(); }

		String getName() const { return name; }
		void setName(const String& name);
		virtual void nameChanged() {}

		virtual Colour getDefaultColour() { return Colour(); }
		Colour getColour() const { return colour; }
		void setColour(Colour colour);
		virtual void colourChanged() {}

		void setProcessLock(CriticalSection& processLockToUse, bool takeOwnership = false) { processLock.set(&processLockToUse, takeOwnership); }
		inline const CriticalSection& getProcessLock() const { return *processLock; }

		virtual int getLatencySamples() const;
		virtual int getLatencySamplesInTree(bool includeOwnLatency = true) const = 0;
		virtual int getLatencyDiffForNode(const MixerNode* node) const = 0;

		void setProcessingState(ProcessingState* newState, bool takeOwnership = true) { processingState.set(newState, takeOwnership); }
		inline ProcessingState* getProcessingState() const { return processingState; }

	protected:
		OptionalScopedPointer<ProcessingState> processingState;
		ListenerList<Listener> listeners;
		AudioClipCache clipCache;

	private:
		String name;
		Colour colour;
		OptionalScopedPointer<CriticalSection> processLock;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerNode)
	};
}

