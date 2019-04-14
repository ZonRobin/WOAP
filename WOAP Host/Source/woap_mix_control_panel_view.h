#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_audio_core_listener.h"
#include "woap_view.h"
#include "woap_track_view.h"
#include "woap_flex_list.h"

namespace WOAP
{
	namespace GUI
	{
		class MixControlPanelView : public View<AudioCore>
		{
		public:
			MixView(AudioCore* core);
			virtual ~MixView();

			virtual void trackAdded(Track* newTrack) override;
			virtual void trackRemoved(Track* oldTrack) override;
			virtual void audioCoreWillBeDeleted() override;

			virtual void mouseDown(const MouseEvent& event) override;

			void paint(Graphics& g) override;
			void resized() override;

			virtual void removeTrackRequested(TrackView* view) override;

		protected:
			void addTrack(Track* newTrack, bool repaint = true);
			void removeTrack(Track* oldTrack);

			ScopedPointer<FlexList<TrackView>> trackList;
			ScopedPointer<ButtonView> muteButton;
			ScopedPointer<ButtonView> soloButton;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixView)
		};
	}
}
