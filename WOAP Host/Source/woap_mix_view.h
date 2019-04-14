#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_audio_core.h"
#include "woap_track_view.h"
#include "woap_flex_list.h"

namespace WOAP
{
	class Track;

	namespace GUI
	{
		class MixView : public Component, public AudioCore::Listener, public TrackView::Listener, public FlexList<TrackView>::Listener
		{
		public:
			class ControlPanel;

			MixView();
			virtual ~MixView();

			void resized() override;

			virtual void trackAdded(Track* newTrack, int position) override;
			virtual void trackRemoved(Track* oldTrack) override;
			virtual void trackMoved(Track*, int oldPosition, int newPosition) override;
			virtual void aboutToDelete(AudioCore*) override {}

			void setSelectedTrack(Track* tr);

			virtual void removeTrackRequested(TrackView* view) override;
			virtual void swapGestureCallback(FlexList<TrackView>* list, int sourceIndex, int targetIndex) override;
			virtual void clickedToEmptySpace(FlexList<TrackView>*, const MouseEvent&) override;

		protected:
			void addTrack(Track* newTrack, int position, bool repaint = true);
			void removeTrack(Track* oldTrack);

			ScopedPointer<FlexList<TrackView>> trackList;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixView)
		};
	}
}
