#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace GUI
	{
		class DragHandle : public Component
		{
		public:
			DragHandle();
			~DragHandle();

			void paint(Graphics& g) override;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DragHandle)
		};
	}
}
