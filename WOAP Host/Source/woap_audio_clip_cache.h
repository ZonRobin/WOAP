#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_audio_buffer_extended.h"

namespace WOAP
{
	class AudioClipCache
	{
	public:
		AudioClipCache() {}
		virtual ~AudioClipCache() {}

		inline uint32 getLastClipTimeMilliseconds() const noexcept { return lastClipTimeMs; }

		inline void updateClipTime() noexcept { lastClipTimeMs = Time::getApproximateMillisecondCounter(); }
		inline void resetClipTime() noexcept { lastClipTimeMs = 0; }

		template <typename FloatType>
		inline void testBuffer(const AudioBufferExtended<FloatType>& buffer)
		{
			/* -3 dBFS clip threshold */
			if (buffer.containsSampleOver((FloatType)0.75))
				updateClipTime();
		}

	protected:
		uint32 lastClipTimeMs = 0;

	private:
		JUCE_LEAK_DETECTOR(AudioClipCache)
	};
}