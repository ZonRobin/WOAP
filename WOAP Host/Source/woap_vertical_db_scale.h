#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace GUI
	{
		class VerticalDbScale : public Component
		{
		public:
			VerticalDbScale();
			~VerticalDbScale();

			void paint(Graphics& g) override;

			void setRange(float newMin, float newMax) { valueMin = newMin; valueMax = newMax; repaint(); }
			void setSkewFactor(float newSkew) { skewFactor = newSkew; repaint(); }
			void setMargin(int newMargin) { margin = newMargin; repaint(); }

		protected:
			float valueMin = -100.0f;
			float valueMax = 0;
			float skewFactor = 1;
			int margin = 0;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VerticalDbScale)
		};
	}
}