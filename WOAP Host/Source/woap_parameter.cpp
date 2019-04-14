#include "woap_parameter.h"

using namespace WOAP;

Parameter::Parameter(const String& parameterID, const String& name)
	: parameterID(parameterID), name(name)
{

}

Parameter::~Parameter()
{
	listeners.call(&Listener::parameterWillBeDeleted, this);
}

void Parameter::setValueNotifyingListeners(float newValue)
{
	setValue(newValue);
	listeners.call(&Listener::parameterChanged, this);
}