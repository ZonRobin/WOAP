#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace GUI
	{
		class Window : public DocumentWindow
		{
		public:
			class CloseWindowListener
			{
			public:
				virtual void closeWindowTriggered(Window* closedWindow) = 0;
			};

			
			Window(CloseWindowListener* own, const String& name,
				Colour backgroundColour,
				bool escapeKeyTriggersCloseButton,
				bool addToDesktop = true);
			virtual ~Window();

			void requestWindowClose() { owner->closeWindowTriggered(this); }
			void closeButtonPressed() override;

		private:
			CloseWindowListener* owner;

			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Window)
		};
	}
}

