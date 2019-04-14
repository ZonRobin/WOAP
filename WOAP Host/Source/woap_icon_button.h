#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace GUI
	{
		class IconButton : public Button
		{
		public:
			IconButton(const Icon& iconToUse);
			~IconButton();

			virtual void paintButton(Graphics& g,
				bool isMouseOverButton,
				bool isButtonDown) override;

			void setShowBackground(bool show) { showBackground = show; repaint(); }

		protected:
			Icon iconPainted;
			bool showBackground = true;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IconButton)
		};
	}
}