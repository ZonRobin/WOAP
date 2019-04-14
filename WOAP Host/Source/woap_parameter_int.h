#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_parameter.h"

namespace WOAP
{
	class ParameterInt : public Parameter
	{
	public:
		/* Creates an ParameterInt with an ID, name, and range */
		ParameterInt(const String& parameterID, const String& name,
			int minValue, int maxValue,
			int defaultValue);

		/* Destructor */
		virtual ~ParameterInt();

		/* Returns the parameter's current value as an integer */
		int get() const noexcept { return roundToInt(value); }

		/* Returns the parameter's current value as an integer */
		operator int() const noexcept { return get(); }

		/* Changes the parameter's current value to a new integer */
		ParameterInt& operator= (int newValue);

		/* Returns the parameter's range */
		Range<int> getRange() const noexcept { return Range<int>(minValue, maxValue); }

		/* Get parameter default value */
		float getDefaultValue() const override;

		/* Get value for text */
		String getText(float, int) const override;

		/* Get text from value */
		float getValueForText(const String&) const override;

	protected:
		int minValue;
		int maxValue;
		float value;
		float defaultValue;

		/* Get parameter value (0 - 1) */
		float getValue() const override;

		/* Set parameter value */
		void setValue(float newValue) override;

		/* Return value in between this parameter range */
		int limitRange(int) const noexcept;

		/* Covert parameter to value from 0 to 1 */
		float convertTo0to1(int) const noexcept;

		/* Convert parameter from 0-1 to full scale */
		int convertFrom0to1(float) const noexcept;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterInt)
	};
}
