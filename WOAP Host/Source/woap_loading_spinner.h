#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace GUI
	{
		class LoadingSpinner : public Component, protected Timer
		{
		public:
			LoadingSpinner(bool dark = false);
			virtual ~LoadingSpinner();

			void paint(Graphics& g) override;
			virtual void timerCallback() override;

			virtual void visibilityChanged() override;

		protected:
			bool dark;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoadingSpinner)
		};
	}
}
