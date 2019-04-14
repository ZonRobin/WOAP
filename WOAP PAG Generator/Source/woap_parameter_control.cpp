#include "woap_parameter_control.h"

ParameterControl::ParameterControl(PluginParameter* parameter)
	: pluginParameter(parameter)
{
	pluginParameter->addListener(this);
}

ParameterControl::ParameterControl(const ParameterControl& other)
{
	pluginParameter = other.pluginParameter;
	pluginParameter->addListener(this);
	colourScheme = other.colourScheme;
	inverted = other.inverted;
}

ParameterControl::~ParameterControl()
{
	pluginParameter->removeListener(this);
}

void ParameterControl::mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel)
{
	if (e.eventTime != lastMouseWheelTime)
	{
		lastMouseWheelTime = e.eventTime;

		if (!e.mods.isAnyMouseButtonDown())
		{
			if (wheel.deltaY < 0 && wheel.isReversed || wheel.deltaY > 0 && !wheel.isReversed)
			{
				mouseWheelUp();
			}
			else
			{
				mouseWheelDown();
			}
		}
	}
}

void ParameterControl::drawOutlineIfFocused(Graphics& g)
{
	if (focused)
	{
		g.setColour(colourScheme.fill.contrasting());
		g.drawRoundedRectangle(getLocalBounds().toFloat(), 5.0f, 2.0f);
	}
}

void ParameterControl::mouseWheelUp()
{
	float newValue = -1.0f;

	if (pluginParameter->getType() == PluginParameter::Type::Float)
	{
		newValue = jmin(pluginParameter->getValue() + 0.05f, 1.0f);
	}
	else
	{
		const SortedSet<float>& registeredValues = pluginParameter->getRegisteredValues();
		for (float value : registeredValues)
		{
			if (pluginParameter->getValue() < value)
			{
				newValue = value;
				break;
			}
		}
	}

	if (newValue != -1)
	{
		valueListeners.call(&ParameterControl::ValueListener::parameterControlValueChanged, this, newValue);
		repaint();
	}
}

void ParameterControl::mouseWheelDown()
{
	float newValue = -1.0f;

	if (pluginParameter->getType() == PluginParameter::Type::Float)
	{
		newValue = jmax(pluginParameter->getValue() - 0.05f, 0.0f);
	}
	else
	{
		const SortedSet<float>& registeredValues = pluginParameter->getRegisteredValues();
		for (float value : registeredValues)
		{
			if (pluginParameter->getValue() <= value)
			{
				break;
			}

			newValue = value;
		}
	}

	if (newValue != -1)
	{
		valueListeners.call(&ParameterControl::ValueListener::parameterControlValueChanged, this, newValue);
		repaint();
	}
}
