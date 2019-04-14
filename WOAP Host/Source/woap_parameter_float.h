#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_parameter.h"

namespace WOAP 
{
	class  ParameterFloat : public Parameter
	{
	public:
		/* Creates a RzAudioParameterFloat with an ID, name, and range */
		ParameterFloat(const String& parameterID,
			const String& name,
			NormalisableRange<float> normalisableRange,
			float defaultValue);

		/* Creates a RzAudioParameterFloat with an ID, name, and range */
		ParameterFloat(const String& parameterID,
			const String& name,
			float minValue,
			float maxValue,
			float defaultValue);

		/* Destructor */
		virtual ~ParameterFloat();

		/* Returns the parameter's current value */
		float get() const noexcept { return value; }

		/* Returns the parameter's current value */
		operator float() const noexcept { return value; }

		/* Changes the parameter's current value */
		ParameterFloat& operator= (float newValue);

		float getMinValue() const;
		float getMaxValue() const;

		/* Get parameter default value */
		float getDefaultValue() const override;

		/* Get number of parameter steps */
		int getNumSteps() const override;

		/* Get value for text */
		String getText(float, int) const override;

		/* Get text from value */
		float getValueForText(const String&) const override;

	protected:
		float value;
		float defaultValue;

		/* Provides access to the parameter's range */
		NormalisableRange<float> range;

		/* Get parameter value (0 - 1) */
		float getValue() const override;

		/* Set parameter value */
		void setValue(float newValue) override;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterFloat)
	};
}
