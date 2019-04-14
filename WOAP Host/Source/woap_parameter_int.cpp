#include "woap_parameter_int.h"

namespace WOAP
{
	ParameterInt::ParameterInt(const String& pid, const String& nm, int mn, int mx, int def)
		: Parameter(pid, nm),
		minValue(mn), maxValue(mx),
		value((float)def),
		defaultValue(convertTo0to1(def))
	{
		jassert(minValue < maxValue);
	}

	ParameterInt::~ParameterInt() 
	{
	
	}

	float ParameterInt::getValue() const
	{
		return convertTo0to1(roundToInt(value));
	}

	void ParameterInt::setValue(float newValue)
	{
		value = (float)convertFrom0to1(newValue);
	}

	float ParameterInt::getDefaultValue() const
	{
		return defaultValue;
	}

	float ParameterInt::getValueForText(const String& text) const
	{
		return convertTo0to1(text.getIntValue());
	}

	String ParameterInt::getText(float v, int /*length*/) const
	{
		return String(convertFrom0to1(v));
	}

	ParameterInt& ParameterInt::operator= (int newValue)
	{
		if (get() == newValue)
			return *this;

		if (newValue >= maxValue) {
			setValueNotifyingListeners(1);
			return *this;
		}

		if (newValue <= minValue) {
			setValueNotifyingListeners(0);
			return *this;
		}

		setValueNotifyingListeners(convertTo0to1(newValue));
		return *this;
	}

	int ParameterInt::limitRange(int v) const noexcept
	{
		return jlimit(minValue, maxValue, v);
	}

	float ParameterInt::convertTo0to1(int v) const noexcept
	{
		return (limitRange(v) - minValue) / (float)(maxValue - minValue);
	}

	int ParameterInt::convertFrom0to1(float v) const noexcept
	{
		return limitRange(roundToInt((v * (float)(maxValue - minValue)) + minValue));
	}
}