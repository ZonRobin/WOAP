#include "woap_plugin_parameter_toolbar.h"
#include "woap_plugin_parameter.h"
#include "woap_parameter_control_rotary_slider.h"
#include "woap_parameter_control_linear_slider.h"
#include "woap_parameter_control_button.h"
#include "woap_entity_manager.h"
#include "woap_colour_scheme.h"
#include "woap_view_editor.h"
#include "woap_look_and_feel.h"

PluginParameterToolbar::PluginParameterToolbar()
{
	EntityManager::getMainComponent()->addListener(this);
	EntityManager::getMainComponent()->getViewEditor()->addListener(this);
}

PluginParameterToolbar::~PluginParameterToolbar()
{
	EntityManager::getMainComponent()->removeListener(this);

	if (EntityManager::getMainComponent()->getViewEditor() != nullptr)
	{
		EntityManager::getMainComponent()->getViewEditor()->removeListener(this);
	}
}

void PluginParameterToolbar::paint(Graphics & g)
{
	const Colour textColour = WOAPLookAndFeel::getTextColour();
	const Colour highlightColour = WOAPLookAndFeel::getHeaderBackgroundColour();

	const Rectangle<int> bounds = getLocalBounds();
	const Rectangle<int> headingBounds = bounds.withHeight(30);
	
	if (activeParameter == nullptr)
	{
		g.setColour(textColour);
		if (EntityManager::getMainComponent()->getPlugin() == nullptr)
			g.drawFittedText("Load plugin by drag&drop or using File menu", bounds, Justification::centred, 1);
		else
			g.drawFittedText("Move a plugin parameter", bounds, Justification::centred, 1);
	}
	else
	{
		g.setColour(highlightColour);
		g.fillRect(headingBounds);
		g.setColour(textColour);
		g.drawFittedText(String("Parameter " + activeParameter->getRef()), headingBounds, Justification::centred, 1);

		g.setColour(textColour);
		const Rectangle<int> parameterTypeBounds = bounds.withTop(35).withHeight(20);
		switch (activeParameter->getType())
		{
		case PluginParameter::Type::Float:
			g.drawFittedText(String("Type: continuous"), parameterTypeBounds, Justification::centred, 1);
			break;
		case PluginParameter::Type::Switch:
			g.drawFittedText(String("Type: switch (" + String(activeParameter->getNumSteps()) + ")"), parameterTypeBounds, Justification::centred, 1);
			break;
		default:
			g.drawFittedText(String("Move the parameter to learn its type"), parameterTypeBounds, Justification::centred, 1);
		}

		if (activeParameter->getType() != PluginParameter::Type::Unknown)
		{
			String valueString(activeParameter->getValue() * 100, 2);
			if (activeParameter->getValueText().isNotEmpty())
			{
				valueString += " (" + activeParameter->getValueText() + ")";
			}

			g.setColour(textColour);
			g.drawFittedText(valueString, bounds.withTop(60).withHeight(20), Justification::centred, 1);

			Rectangle<int> valueBarBounds = bounds.withTop(80).withHeight(5);
			valueBarBounds.setWidth((int)(valueBarBounds.getWidth() * activeParameter->getValue()));
			g.setColour(highlightColour);
			g.fillRect(valueBarBounds);

			g.setColour(textColour);
			g.drawFittedText("Drag&drop control to the editor area", bounds.withTop(90).withHeight(30), Justification::centred, 1);
			g.drawSingleLineText("Fill", 30, 225);
			g.drawSingleLineText("Accent", 30, 285);
		}	
	}
}

void PluginParameterToolbar::pluginWillBeDeleted()
{
	activeParameter = nullptr;
	activeControl = nullptr;
	removeParameterControls();

	repaint();
}

void PluginParameterToolbar::pluginLoaded(AudioProcessor*, PluginDescription*)
{
}

void PluginParameterToolbar::pluginEditorWillBeDeleted()
{
}

void PluginParameterToolbar::pluginEditorCreated(AudioProcessorEditor*)
{
}

void PluginParameterToolbar::pluginParameterTouched(PluginParameter* pluginParameter)
{
	if (activeParameter == pluginParameter)
		return;

	if (activeParameter != nullptr)
	{
		activeParameter->removeListener(this);
		removeParameterControls();
	}

	if (pluginParameter == nullptr)
	{
		activeParameter = nullptr;
		activeControl = nullptr;
		removeParameterControls();
		repaint();
		return;
	}

	activeParameter = pluginParameter;
	activeParameter->addListener(this);

	activeControl = nullptr;
	for (ParameterControl* control : EntityManager::getMainComponent()->getViewEditor()->getParameterControls())
	{
		if (control->getPluginParameter() == pluginParameter)
		{
			activeControl = control;
			break;
		}
	}

	rotarySlider = new ParameterControlRotarySlider(pluginParameter);
	rotarySlider->setBounds(40, 120, 50, 50);
	rotarySlider->addValueListener(this);
	addChildComponent(rotarySlider);

	verticalSlider = new ParameterControlLinearSlider(ParameterControlLinearSlider::Orientation::Vertical, pluginParameter);
	verticalSlider->setBounds(100, 120, 20, 50);
	verticalSlider->addValueListener(this);
	addChildComponent(verticalSlider);

	verticalNoBarSlider = new ParameterControlLinearSlider(ParameterControlLinearSlider::Orientation::Vertical, pluginParameter);
	verticalNoBarSlider->setBounds(125, 120, 20, 50);
	verticalNoBarSlider->addValueListener(this);
	verticalNoBarSlider->showValueBar(false);
	addChildComponent(verticalNoBarSlider);

	horizontalSlider = new ParameterControlLinearSlider(ParameterControlLinearSlider::Orientation::Horizontal, pluginParameter);
	horizontalSlider->setBounds(150, 147, 100, 20);
	horizontalSlider->addValueListener(this);
	addChildComponent(horizontalSlider);

	horizontalNoBarSlider = new ParameterControlLinearSlider(ParameterControlLinearSlider::Orientation::Horizontal, pluginParameter);
	horizontalNoBarSlider->setBounds(150, 123, 100, 20);
	horizontalNoBarSlider->addValueListener(this);
	horizontalNoBarSlider->showValueBar(false);
	addChildComponent(horizontalNoBarSlider);

	buttonControl = new ParameterControlButton(pluginParameter);
	buttonControl->setBounds(60, 130, 30, 30);
	buttonControl->addValueListener(this);
	addChildComponent(buttonControl);

	inversionButton = new ToggleButton("Inverted");
	inversionButton->setBounds(220, 34, 75, 22);
	inversionButton->setToggleState(activeControl != nullptr ? activeControl->isInverted() : false, NotificationType::dontSendNotification);
	inversionButton->addListener(this);
	addChildComponent(inversionButton);

	const LookAndFeel& lf = LookAndFeel::getDefaultLookAndFeel();
	ColourPicker* picker = EntityManager::getMainComponent()->getViewEditor()->getColourPicker();

	colourInputFill = new ColourInput();
	colourInputFill->setBounds(100, 200, 50, 50);
	colourInputFill->addListener(this);
	colourInputFill->setActiveColour(activeControl != nullptr ? activeControl->getColourScheme().fill : lf.findColour(Slider::ColourIds::trackColourId), false);
	colourInputFill->setColourPickerToUse(picker);
	addChildComponent(colourInputFill);

	colourInputAccent = new ColourInput();
	colourInputAccent->setBounds(100, 260, 50, 50);
	colourInputAccent->addListener(this);
	colourInputAccent->setActiveColour(activeControl != nullptr ? activeControl->getColourScheme().accent : lf.findColour(Slider::ColourIds::thumbColourId), false);
	colourInputAccent->setColourPickerToUse(picker);
	addChildComponent(colourInputAccent);

	colourChanged();
	invertedChanged();
	activeControlChanged();
	typeChanged(pluginParameter->getType());
	numStepsChanged(pluginParameter->getNumSteps());
	repaint();
}

void PluginParameterToolbar::numStepsChanged(int newNumSteps)
{
	if (newNumSteps > 2 || activeParameter->getType() == PluginParameter::Type::Float)
	{
		rotarySlider->setVisible(true);
		buttonControl->setVisible(false);
	}
	else if (newNumSteps == 2)
	{
		rotarySlider->setVisible(false);
		buttonControl->setVisible(true);
	}

	repaint();
}

void PluginParameterToolbar::typeChanged(PluginParameter::Type newType)
{
	if (newType == PluginParameter::Type::Unknown)
		return;

	verticalSlider->setVisible(true);
	verticalNoBarSlider->setVisible(true);
	horizontalSlider->setVisible(true);
	horizontalNoBarSlider->setVisible(true);
	colourInputFill->setVisible(true);
	colourInputAccent->setVisible(true);
	inversionButton->setVisible(true);

	repaint();
}

void PluginParameterToolbar::parameterControlCreated(ParameterControl* control)
{
	activeControl = control;
	activeControlChanged();
}

void PluginParameterToolbar::parameterControlValueChanged(ParameterControl* control, float newValue)
{
	EntityManager::getMainComponent()->setParameterValue(control->getPluginParameter()->getIndex(), newValue);
}

void PluginParameterToolbar::parameterControlWillDelete(ParameterControl* control)
{
	if (activeControl == control)
	{
		activeControl = nullptr;
		activeControlChanged();
	}
}

void PluginParameterToolbar::parameterChanged(PluginParameter*)
{
	repaint();
}

void PluginParameterToolbar::colourSelected(ColourInput*, const Colour&)
{
	colourChanged();
}

void PluginParameterToolbar::colourChanged()
{
	ColourScheme scheme(colourInputAccent->getActiveColour(), colourInputFill->getActiveColour());

	if (activeControl != nullptr)
	{
		activeControl->setColourScheme(scheme);
	}

	rotarySlider->setColourScheme(scheme);
	buttonControl->setColourScheme(scheme);
	verticalSlider->setColourScheme(scheme);
	verticalNoBarSlider->setColourScheme(scheme);
	horizontalSlider->setColourScheme(scheme);
	horizontalNoBarSlider->setColourScheme(scheme);
}

void PluginParameterToolbar::invertedChanged()
{
	bool newInverted = inversionButton->getToggleState();

	if (activeControl != nullptr)
	{
		activeControl->setInverted(newInverted);
	}

	rotarySlider->setInverted(newInverted);
	buttonControl->setInverted(newInverted);
	verticalSlider->setInverted(newInverted);
	verticalNoBarSlider->setInverted(newInverted);
	horizontalSlider->setInverted(newInverted);
	horizontalNoBarSlider->setInverted(newInverted);
}

void PluginParameterToolbar::activeControlChanged()
{
	const bool activeControlExists = activeControl == nullptr;

	rotarySlider->setDragAndDropEnabled(activeControlExists);
	buttonControl->setDragAndDropEnabled(activeControlExists);
	verticalSlider->setDragAndDropEnabled(activeControlExists);
	verticalNoBarSlider->setDragAndDropEnabled(activeControlExists);
	horizontalSlider->setDragAndDropEnabled(activeControlExists);
	horizontalNoBarSlider->setDragAndDropEnabled(activeControlExists);
}

void PluginParameterToolbar::buttonClicked(Button*)
{
	invertedChanged();
}

void PluginParameterToolbar::removeParameterControls()
{
	rotarySlider = nullptr;
	horizontalSlider = nullptr;
	horizontalNoBarSlider = nullptr;
	verticalSlider = nullptr;
	verticalNoBarSlider = nullptr;
	buttonControl = nullptr;
	colourInputFill = nullptr;
	colourInputAccent = nullptr;
	inversionButton = nullptr;
}
