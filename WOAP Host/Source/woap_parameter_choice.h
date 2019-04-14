#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_parameter.h"

namespace WOAP
{
	class  ParameterChoice : public Parameter
	{
	public:
		/** Creates a AudioParameterChoice with an ID, name, and list of items.
		On creation, its value is set to the default index.
		*/
		ParameterChoice(const String& parameterID, 
			const String& name,
			const StringArray& choices,
			int defaultItemIndex);

		/** Destructor. */
		virtual ~ParameterChoice();

		/** Returns the current index of the selected item. */
		int getIndex() const noexcept { return roundToInt(value); }

		bool isSelected(int index) const noexcept { return index == getIndex(); }

		/** Returns the current index of the selected item. */
		operator int() const noexcept { return getIndex(); }

		/** Returns the name of the currently selected item. */
		String getCurrentChoiceName() const noexcept { return choices[getIndex()]; }
		/** Returns the name of the currently selected item. */
		operator String() const noexcept { return getCurrentChoiceName(); }

		/** Changes the selected item to a new index. */
		ParameterChoice& operator= (int newValue);

		void setSelectedIndex(int newSelectedItemIndex);

		/** Provides access to the list of choices that this parameter is working with. */
		const StringArray choices;

		float getDefaultValue() const override;
		int getNumSteps() const override;
		String getText(float, int) const override;
		float getValueForText(const String&) const override;

	protected:
		float value;
		float defaultValue;

		float getValue() const override;
		void setValue(float newValue) override;

		int limitRange(int) const noexcept;
		float convertTo0to1(int) const noexcept;
		int convertFrom0to1(float) const noexcept;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterChoice)
	};
}

