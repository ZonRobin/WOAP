#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_track.h"
#include "woap_colour_palette.h"

namespace WOAP
{
	namespace GUI
	{
		class SimpleCallOutBox;

		class TrackColourSelector : public Component, public Track::Listener, public Button::Listener, public ColourPalette::Listener
		{
		public:
			TrackColourSelector(Track* track = nullptr);
			virtual ~TrackColourSelector();

			void setTrack(Track* track);

			void paint(Graphics& g) override;
			void resized() override;

			virtual void nameChanged(MixerNode*) override {}
			virtual void colourChanged(MixerNode*) override {}
			virtual void aboutToDelete(MixerNode*) override;

			virtual void buttonClicked(Button*) override;
			virtual void paletteColourSelected(Colour colour) override;

			void openPalette();

		protected:
			Track* track = nullptr;
			ScopedPointer<TextButton> selectColourButton;
			WeakReference<SimpleCallOutBox> colourPaletteCallOut;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackColourSelector)
		};
	}
}
