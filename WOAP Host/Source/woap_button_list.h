#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_flex_list.h"

using std::function;
using std::initializer_list;

namespace WOAP
{
	namespace GUI
	{
		class ButtonList : public FlexList<TextButton>
		{
		public:
			struct ButtonParameters
			{
				int id;
				String text;
			};

			ButtonList(int buttonHeight, int listWidth, const initializer_list<ButtonParameters>& buttonParameters, const function<void(ButtonList*, int)>& callback);
			ButtonList(int buttonHeight, int listWidth, const Array<ButtonParameters>& buttonParameters, const function<void(ButtonList*, int)>& callback);
			~ButtonList();
			
			ButtonList* fitToHeight(int maxHeight);

			virtual void mouseUp(const MouseEvent& event);

		protected:
			function<void(ButtonList*, int)> callback;

			void addButton(int buttonId, const String& buttonText, int buttonHeight, bool refresh = true);

			class ButtonWithId : public TextButton
			{
			public:
				ButtonWithId(int idToUse, const String& text) 
					: id(idToUse), TextButton(text) 
				{
					setMouseCursor(MouseCursor::PointingHandCursor);
				}
				~ButtonWithId() {}

				int id;
			};

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonList)
		};
	}
}