#include "woap_plugin_parameter.h"
#include "woap_exceptions.h"

const int PluginParameter::MaxNumSteps = 20;

PluginParameter::PluginParameter(int indexToUse, String refToUse)
	: index(indexToUse), ref(refToUse)
{
	jassert(indexToUse >= 0);
}

PluginParameter::~PluginParameter()
{

}

void PluginParameter::setValueAndText(float newValue, String newValueText)
{
	value = newValue;
	valueText = newValueText;

	registerValue(newValue);
	listeners.call(&Listener::parameterChanged, this);
}

XmlElement* PluginParameter::createXml() const
{
	XmlElement* param = new XmlElement("parameter");
	param->setAttribute("ref", ref);

	if (type == Type::Float || type == Type::Unknown || (type == Type::Switch && registeredValues.size() < 2))
	{
		param->setAttribute("type", "float");
	}
	else
	{
		param->setAttribute("type", "switch");

		if (registeredValues.size() > 2)
		{
			for (float val : registeredValues)
			{
				XmlElement* step = new XmlElement("step");
				step->setAttribute("value", val);
				param->addChildElement(step);
			}
		}
	}

	return param;
}

void PluginParameter::registerValue(float newValue)
{
	if (type == PluginParameter::Type::Float)
	{
		return;
	}

	if (!registeredValues.add(newValue))
	{
		return;
	}

	listeners.call(&PluginParameter::Listener::numStepsChanged, registeredValues.size());
	updateTypeBySteps();
}

void PluginParameter::updateTypeBySteps()
{
	const int numSteps = registeredValues.size();
	PluginParameter::Type newType;

	if (numSteps <= 1)
		newType = PluginParameter::Type::Unknown;
	else if (numSteps <= PluginParameter::MaxNumSteps)
		newType = PluginParameter::Type::Switch;
	else
		newType = PluginParameter::Type::Float;

	if (type != newType)
	{
		type = newType;
		listeners.call(&PluginParameter::Listener::typeChanged, newType);
	}
}

PluginParameter* PluginParameter::fromXml(int index, const XmlElement& parameterElement)
{
	const String ref = parameterElement.getStringAttribute("ref");
	const String type = parameterElement.getStringAttribute("type");
	
	if (!parameterElement.hasTagName("parameter")) throw ParsingException("Parameter tag name not recognized: " + parameterElement.getTagName());
	if (ref.isEmpty()) throw ParsingException("Parameter reference is empty");
	if (type.isEmpty()) throw ParsingException("Parameter type is empty");
	if (type != "float" && type != "switch") throw ParsingException("Parameter type not recognized: " + type);
	
	ScopedPointer<PluginParameter> parameter(new PluginParameter(index, ref));
	
	if (type == "switch")
	{
		const int numStepElements = parameterElement.getNumChildElements();
		
		if (numStepElements == 0)
		{
			parameter->registeredValues.add(0);
			parameter->registeredValues.add(1.0f);
		}
		else
		{
			forEachXmlChildElement(parameterElement, stepElement)
			{
				if (!stepElement->hasTagName("step")) throw ParsingException("Parameter step tag name not recognized: " + stepElement->getTagName());
				if (!stepElement->hasAttribute("value")) throw ParsingException("Parameter step has no value attribute");

				double value = stepElement->getDoubleAttribute("value", -1);
				if (value > 1 || value < 0) throw ParsingException("Parameter step value invalid: " + String(value));

				parameter->registeredValues.add((float)value);
			}
		}

		parameter->type = PluginParameter::Type::Switch;
	}
	else
	{
		parameter->type = PluginParameter::Type::Float;
	}
	
	return parameter.release();
}

