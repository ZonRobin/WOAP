#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_icon_button.h"

namespace WOAP
{
	namespace GUI
	{
		class ThrashButton : public IconButton, public DragAndDropTarget
		{
		public:
			ThrashButton();
			~ThrashButton();

			virtual bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
			virtual void itemDragEnter(const SourceDetails& dragSourceDetails);
			virtual void itemDropped(const SourceDetails& dragSourceDetails) override;
			virtual void itemDragExit(const SourceDetails& dragSourceDetails);

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThrashButton)
		};
	}
}