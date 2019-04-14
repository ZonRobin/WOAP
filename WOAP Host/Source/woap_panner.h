#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	class ParameterFloat;

	namespace GUI
	{
		class Panner : public Slider
		{
		public:
			Panner();
			virtual ~Panner();

			virtual void paint(Graphics& g) override;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Panner)
		};
	}
}
