#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_parameter.h"

namespace WOAP 
{
	class ParameterBool : public Parameter
	{
	public:
		/* Creates a AudioParameterBool with an ID and name */
		ParameterBool(const String& parameterID, const String& name, bool defaultValue);

		/* Destructor */
		virtual ~ParameterBool();

		/* Get parameter boolean value */
		bool get() const noexcept { return value >= 0.5f; }

		/* Get parameter boolean value */
		operator bool() const noexcept { return get(); }

		/* Set parameter value and notify host */
		ParameterBool& operator= (bool newValue);

		/* Toggles the value of the parameter */
		void toggle();

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

		/* Get parameter value (0 - 1) */
		float getValue() const override;

		/* Set parameter value */
		void setValue(float newValue) override;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterBool)
	};
}
