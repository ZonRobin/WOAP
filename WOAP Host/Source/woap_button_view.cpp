#include "woap_button_view.h"
#include "woap_track.h"
#include "woap_look_and_feel.h"
#include "woap_audio_processor_player_double.h"

using namespace WOAP;
using namespace WOAP::GUI;

ButtonView::ButtonView(const String& text, ParameterBool* p)
	: parameter(nullptr), TextButton(text)
{
	setMouseCursor(MouseCursor::PointingHandCursor);
	setParameter(p);
}

ButtonView::~ButtonView()
{
	if (parameter != nullptr)
		parameter->removeListener(this);
}

void ButtonView::setParameter(ParameterBool* param)
{
	if (parameter != nullptr) 
		parameter->removeListener(this);

	parameter = param;

	if (param != nullptr) 
	{
		param->addListener(this);
		setToggleState(param->get(), NotificationType::dontSendNotification);
	}
}

void ButtonView::mouseUp(const MouseEvent& ev)
{
	Button::mouseUp(ev);
	
	if (ev.mods.isLeftButtonDown() && !ev.source.hasMouseMovedSignificantlySincePressed())
	{
		clickedWithoutDrag();
	}
}

void ButtonView::clickedWithoutDrag()
{
	if (parameter != nullptr)
		parameter->toggle();
}

void ButtonView::parameterChanged(Parameter*)
{
	setToggleState(parameter->get(), NotificationType::dontSendNotification);
}


/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */

MuteButtonView::MuteButtonView(const String& text, Track* tr)
	: ButtonView(text)
{
	setColour(TextButton::ColourIds::buttonOnColourId, CustomColours::red);
	setColour(TextButton::ColourIds::textColourOnId, Colours::white);

	setTrack(tr);
}

void MuteButtonView::setTrack(Track* tr)
{
	if (tr == nullptr)
		setParameter(nullptr);
	else
		setParameter(tr->getMuteParameter());
}

/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */

SoloButtonView::SoloButtonView(const String& text, Track* tr)
	: ButtonView(text)
{
	setColour(TextButton::ColourIds::buttonOnColourId, CustomColours::yellow);

	setTrack(tr);
}

void SoloButtonView::setTrack(Track* tr)
{
	if (tr == nullptr)
		setParameter(nullptr);
	else
		setParameter(tr->getSoloParameter());
}

/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */

RecordArmButtonView::RecordArmButtonView(const String& text, Track* tr)
	: ButtonView(text)
{
	setColour(TextButton::ColourIds::buttonOnColourId, CustomColours::lightBlue);

	setTrack(tr);
}

void RecordArmButtonView::setTrack(Track* tr)
{
	if (tr == nullptr)
		setParameter(nullptr);
	else
		setParameter(tr->getRecordArmParameter());
}

/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */

BypassButtonView::BypassButtonView(AudioProcessorPlayerDouble* pl, const String& text)
	: ButtonView(text, pl->getBypassParameter()), plugin(pl)
{
	setColour(TextButton::ColourIds::buttonOnColourId, CustomColours::red);
	setColour(TextButton::ColourIds::textColourOnId, Colours::white);
}

/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */

ActiveButtonView::ActiveButtonView(AudioProcessorPlayerDouble* pl, const String& text)
	: ButtonView(text, pl->getActiveStateParameter()), plugin(pl)
{
	setColour(TextButton::ColourIds::buttonOnColourId, CustomColours::green);
}

/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */
/* ---------------------------------------------------------------------------------- */