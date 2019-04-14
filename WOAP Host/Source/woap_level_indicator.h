#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	class Track;

	namespace GUI
	{
		class LevelIndicator : public Component, protected MultiTimer
		{
		public:
			LevelIndicator();
			virtual ~LevelIndicator();

			void paint(Graphics& g) override;
			
			void setValues(float valueL, float valueR, bool clipping = false, bool soloed = false);
			void setValuesNoAverage(float valueL, float valueR, bool clipping = false, bool soloed = false);
			void setRange(float newMin, float newMax) { valueMin = newMin; valueMax = newMax; repaint(); }
			void setSkewFactor(float newSkew) { skewFactor = newSkew; repaint(); }
			void setAveraging(float newAvg) { averaging = newAvg; }

			float levelToHeight(float gain) const;

			virtual void timerCallback(int timerID) override;

		protected:
			float values[2] = { 0, 0 };
			bool clipping = false;
			bool soloed = false;
			float valueMin = -100.0f;
			float valueMax = 0;
			float skewFactor = 1;
			float averaging = 0.5f;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelIndicator)
		};
	}
}
