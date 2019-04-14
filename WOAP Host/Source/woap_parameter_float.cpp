#include "woap_parameter_float.h"

namespace WOAP
{
	ParameterFloat::ParameterFloat(const String& pid, const String& nm, NormalisableRange<float> r, float def)
		: Parameter(pid, nm), range(r), value(def), defaultValue(def)
	{

	}

	ParameterFloat::ParameterFloat(const String& pid, const String& nm, float minValue, float maxValue, float def)
		: Parameter(pid, nm), range(minValue, maxValue), value(def), defaultValue(def)
	{

	}

	ParameterFloat::~ParameterFloat() 
	{
	
	}

	float ParameterFloat::getMinValue() const
	{
		return range.start;
	}

	float ParameterFloat::getMaxValue() const
	{
		return range.end;
	}

	float ParameterFloat::getValue() const
	{
		return range.convertTo0to1(value);
	}

	void ParameterFloat::setValue(float newValue)
	{
		value = range.convertFrom0to1(newValue);
	}

	float ParameterFloat::getDefaultValue() const
	{
		return range.convertTo0to1(defaultValue);
	}

	int ParameterFloat::getNumSteps() const
	{
		return Parameter::getDefaultNumSteps();
	}

	float ParameterFloat::getValueForText(const String& text) const
	{
		return range.convertTo0to1(text.getFloatValue());
	}

	String ParameterFloat::getText(float v, int length) const
	{
		String asText(range.convertFrom0to1(v), 2);
		return length > 0 ? asText.substring(0, length) : asText;
	}

	ParameterFloat& ParameterFloat::operator= (float newValue)
	{
		if (value == newValue)
			return *this;

		if (newValue >= range.end) {
			setValueNotifyingListeners(1);
			return *this;
		}

		if (newValue <= range.start) {
			setValueNotifyingListeners(0);
			return *this;
		}
			
		setValueNotifyingListeners(range.convertTo0to1(newValue));
		return *this;
	}
}