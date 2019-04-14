#include "woap_parameter_choice.h"

using namespace WOAP;

ParameterChoice::ParameterChoice(const String& idToUse, const String& nameToUse,
	const StringArray& c, int def)
	: Parameter(idToUse, nameToUse), choices(c),
	value((float)def),
	defaultValue(convertTo0to1(def))
{
	jassert(choices.size() > 0);
}

ParameterChoice::~ParameterChoice() 
{

}

int ParameterChoice::limitRange(int v) const noexcept 
{ 
	return jlimit(0, choices.size() - 1, v); 
}

float ParameterChoice::convertTo0to1(int v) const noexcept 
{ 
	return jlimit(0.0f, 1.0f, (v + 0.5f) / (float)choices.size()); 
}

int ParameterChoice::convertFrom0to1(float v) const noexcept 
{ 
	return limitRange((int)(v * (float)choices.size())); 
}

float ParameterChoice::getValue() const 
{ 
	return convertTo0to1(roundToInt(value)); 
}

void ParameterChoice::setValue(float newValue) 
{ 
	value = (float)convertFrom0to1(newValue); 
}

float ParameterChoice::getDefaultValue() const 
{ 
	return defaultValue; 
}

int ParameterChoice::getNumSteps() const 
{ 
	return choices.size(); 
}

float ParameterChoice::getValueForText(const String& text) const 
{ 
	return convertTo0to1(choices.indexOf(text)); 
}

String ParameterChoice::getText(float v, int /*length*/) const 
{
	return choices[convertFrom0to1(v)]; 
}

ParameterChoice& ParameterChoice::operator= (int newValue)
{
	setSelectedIndex(newValue);

	return *this;
}

void ParameterChoice::setSelectedIndex(int newSelectedItemIndex)
{
	if (getIndex() != newSelectedItemIndex)
		setValueNotifyingListeners(convertTo0to1(newSelectedItemIndex));
}