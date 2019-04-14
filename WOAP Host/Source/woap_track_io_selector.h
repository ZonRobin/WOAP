#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	class Track;

	namespace GUI
	{
		class SimpleCallOutBox;

		class TrackIOSelector : public Component, public Button::Listener
		{
		public:
			enum Type
			{
				Input,
				Output
			};

			TrackIOSelector(Type type, Track* tr = nullptr);
			virtual ~TrackIOSelector();

			void setTrack(Track* tr);
			virtual void resized() override;

			virtual void buttonClicked(Button*) override;

		protected:
			Type type;
			Track* track = nullptr;
			ScopedPointer<TextButton> selectorButton;
			WeakReference<SimpleCallOutBox> selectorCallOut;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackIOSelector)
		};
	}
}