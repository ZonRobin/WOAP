#include "woap_parameter_bool.h"

namespace WOAP
{
	ParameterBool::ParameterBool(const String& pid, const String& nm, bool defaultValue)
		: Parameter(pid, nm), defaultValue(defaultValue), value(defaultValue ? 1.0f : 0)
	{

	}

	ParameterBool::~ParameterBool() {}

	float ParameterBool::getValue() const
	{
		return value;
	}

	void ParameterBool::setValue(float newValue)
	{
		value = newValue;
	}

	float ParameterBool::getDefaultValue() const
	{
		return defaultValue;
	}

	int ParameterBool::getNumSteps() const
	{
		return 2;
	}

	void ParameterBool::toggle()
	{
		*this = !get();
	}

	float ParameterBool::getValueForText(const String& text) const
	{
		return text.getIntValue() != 0 ? 1.0f : 0.0f;
	}

	String ParameterBool::getText(float v, int) const
	{
		return String((int)(v > 0.5f ? 1 : 0));
	}

	ParameterBool& ParameterBool::operator= (bool newValue)
	{
		if (get() != newValue)
			setValueNotifyingListeners(newValue ? 1.0f : 0.0f);

		return *this;
	}

}