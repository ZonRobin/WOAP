#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	class SoloBus;

	namespace GUI
	{
		class LevelIndicator;
		class SimpleCallOutBox;

		class SoloBusSettingsComponent : public Component, public Timer, public Button::Listener
		{
		public:
			SoloBusSettingsComponent();
			~SoloBusSettingsComponent();

			void resized() override;
			void paint(Graphics& g) override;
			void timerCallback() override;

			virtual void buttonClicked(Button*) override;

		protected:
			SoloBus* soloBus;
			ScopedPointer<TextButton> selectorButton;
			ScopedPointer<LevelIndicator> levelIndicator;
			WeakReference<SimpleCallOutBox> selectorCallOut;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloBusSettingsComponent)
		};

	}
}
