#include "woap_help_box.h"
#include "woap_entity_manager.h"
#include "woap_parameter_automizer.h"
#include "woap_plugin_parameter.h"
#include "woap_look_and_feel.h"

HelpBox::HelpBox()
	: step(Step::LoadPlugin)
{
	EntityManager::getMainComponent()->addListener(this);

	stepForwardButton = new TextButton("Next");
	stepForwardButton->addListener(this);
	addChildComponent(stepForwardButton);
	
	stepBackButton = new TextButton("Back");
	stepBackButton->addListener(this);
	addChildComponent(stepBackButton);

	hintLabel = new Label();
	hintLabel->setEditable(false);
	hintLabel->setJustificationType(Justification::centred);
	addAndMakeVisible(hintLabel);

	if (EntityManager::getMainComponent()->isPluginLoaded())
	{
		pluginLoaded(nullptr, nullptr);
	}
	else
	{
		stepChanged();
	}
}

HelpBox::~HelpBox()
{
	EntityManager::getMainComponent()->removeListener(this);
}

void HelpBox::resized()
{
	Rectangle<int> bounds = getLocalBounds();

	hintLabel->setBounds(bounds.removeFromTop((int)(bounds.getHeight() * 0.75f)));
	stepBackButton->setBounds(bounds.withWidth((int)(bounds.getWidth() / 3.0f)));
	stepForwardButton->setBounds(bounds.withTrimmedLeft((int)(2 * bounds.getWidth() / 3.0f)));
}

void HelpBox::paint(Graphics& g)
{
	g.setColour(WOAPLookAndFeel::getHeaderBackgroundColour());
	g.drawHorizontalLine(0, 0, (float)getWidth());
}

String HelpBox::getMessageForStep(Step st)
{
	switch (st)
	{
	case Step::LoadPlugin:
		return "Drag & drop VST plugin to the window or use File -> Open plugin file to select a file.";
	case Step::ChooseControlElement:
	{
		const String ref = EntityManager::getMainComponent()->getPluginParameter(parameterHandled)->getRef();
		return "Parameter " + ref + " is being moved now. Choose adequate control element from the list above and place it by drag & drop.";
	}
	case Step::ResizeControl:
		return "Resize the control element to fit by dragging its borders.";
	case Step::ChooseColour:
		return "Choose both colors for the parameter control or use a colorpicker - click on Pick and click in the area of VST plugin to pick the color.";
	case Step::AllDone:
		return "Everything is done. Do your fine adjustments and export the result as XML via File - Export As";
	default:
		return String();
	}
}

void HelpBox::runTutorial()
{
	step = Step::LoadPlugin;
	stepChanged();
}

void HelpBox::stepTutorialForward()
{
	if (!canStepForward()) return;

	switch (step)
	{
	case Step::ChooseControlElement:
	{
		step = Step::ResizeControl;
		stepChanged();
		return;
	}
	case Step::ResizeControl:
	{
		step = Step::ChooseColour;
		stepChanged();
		return;
	}
	case Step::ChooseColour:
	{
		startParameterAutomation();
		if (EntityManager::getMainComponent()->getPluginParameters().size() - 1 > parameterHandled)
		{
			++parameterHandled;
			step = Step::ChooseControlElement;
			startParameterAutomation();
		}
		else
		{
			stopParameterAutomation();
			step = Step::AllDone;
		}

		stepChanged();
		return;
	}
	default:
		return;
	}
}

void HelpBox::stepTutorialBack()
{
	if (!canStepBack()) return;

	switch (step)
	{
	case Step::ChooseControlElement:
	{
		if (parameterHandled <= 0)
			return;

		--parameterHandled;
		step = Step::ChooseColour;
		startParameterAutomation();
		stepChanged();
		return;
	}
	case Step::ResizeControl:
	{
		step = Step::ChooseControlElement;
		stepChanged();
		return;
	}
	case Step::ChooseColour:
	{
		step = Step::ResizeControl;
		stepChanged();
		return;
	}
	case Step::AllDone:
	{
		step = Step::ChooseColour;
		startParameterAutomation();
		stepChanged();
		return;
	}
	default:
		return;
	}
}

void HelpBox::stopTutorial()
{
}

bool HelpBox::canStepForward() const
{
	switch (step)
	{
	case Step::ChooseControlElement:
	case Step::ResizeControl:
	case Step::ChooseColour:
		return true;
	default:
		return false;
	}
}

bool HelpBox::canStepBack() const
{
	switch (step)
	{
	case Step::ChooseControlElement:
		return parameterHandled > 0;
	case Step::ResizeControl:
	case Step::ChooseColour:
	case Step::AllDone:
		return true;
	default:
		return false;
	}
}

void HelpBox::stepChanged()
{
	stepForwardButton->setVisible(canStepForward());
	stepBackButton->setVisible(canStepBack());
	hintLabel->setText(getMessageForStep(step), NotificationType::dontSendNotification);

	repaint();
}

void HelpBox::startParameterAutomation()
{	
	EntityManager::getMainComponent()->openPluginEditor();
	automizer = new ParameterAutomizer(parameterHandled);
}

void HelpBox::stopParameterAutomation()
{
	automizer = nullptr;
}

void HelpBox::pluginWillBeDeleted()
{
	step = Step::LoadPlugin;
	stopParameterAutomation();
	parameterHandled = -1;
	stepChanged();
}

void HelpBox::pluginLoaded(AudioProcessor*, PluginDescription*)
{
	if (step == Step::LoadPlugin)
	{
		step = Step::ChooseControlElement;
		parameterHandled = 0;
		startParameterAutomation();
		stepChanged();
	}
}

void HelpBox::pluginEditorWillBeDeleted()
{
}

void HelpBox::pluginEditorCreated(AudioProcessorEditor*)
{
}

void HelpBox::pluginParameterTouched(PluginParameter*)
{
}

void HelpBox::buttonClicked(Button* btn)
{
	if (btn == stepForwardButton)
		stepTutorialForward();
	else if (btn == stepBackButton)
		stepTutorialBack();
}
