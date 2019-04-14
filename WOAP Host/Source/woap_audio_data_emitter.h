#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_audio_buffer_extended.h"

namespace WOAP
{
	class AudioDataEmitter
	{
	public:
		class Listener
		{
		public:
			virtual void audioProcessed(AudioDataEmitter* emitter, const AudioBufferDouble& audioBuffer) = 0;
			virtual void aboutToDelete(AudioDataEmitter* emitter) = 0;
		};

		AudioDataEmitter() {}
		virtual ~AudioDataEmitter() {}

		void addAudioDataListener(Listener* listener) { audioDataListeners.add(listener); }
		void removeAudioDataListener(Listener* listener) { audioDataListeners.remove(listener); }

	protected:
		LockedListenerList<AudioDataEmitter::Listener> audioDataListeners;
	};
}