#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_level_indicator.h"

namespace WOAP
{
	class Track;

	namespace GUI
	{
		class TrackLevelIndicatorView : public LevelIndicator
		{
		public:
			TrackLevelIndicatorView(Track* track = nullptr);
			virtual ~TrackLevelIndicatorView();

			void setTrack(Track* tr);
			virtual void timerCallback(int timerID) override;

		protected:
			Track* track;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLevelIndicatorView)
		};
	}
}