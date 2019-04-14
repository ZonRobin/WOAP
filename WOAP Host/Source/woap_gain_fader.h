#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace GUI
	{
		class GainFader : public Slider
		{
		public:
			GainFader();
			virtual ~GainFader();

			virtual void paint(Graphics& g) override;
			void setThumbOpacity(float newOpacity) { thumbOpacity = newOpacity; repaint(); }

			Rectangle<int> getThumbPosition();

			virtual void mouseDown(const MouseEvent& e) override;
			virtual void mouseDrag(const MouseEvent& e) override;
			virtual void mouseUp(const MouseEvent& e) override;

		protected:
			void paintThumb(Graphics& g);
			float thumbOpacity = 1;
			bool mouseDownOnThumb = false;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainFader)
		};
	}
}
