#include "woap_button_list.h"

using namespace WOAP;
using namespace WOAP::GUI;

ButtonList::ButtonList(int buttonHeight, int listWidth, const initializer_list<ButtonParameters>& buttonParameters, const function<void(ButtonList*, int)>& cb)
	: callback(cb), FlexList<TextButton>(FlexList<TextButton>::Direction::Vertical)
{
	addEmptyPlaceholder(listWidth, buttonHeight, false);

	for (const ButtonParameters& params : buttonParameters)
	{
		addButton(params.id, params.text, buttonHeight, false);
	}
	
	setScrollOnDragEnabled(true);
	setSize(listWidth, jmax(buttonHeight, getIdealSize()));
}

ButtonList::ButtonList(int buttonHeight, int listWidth, const Array<ButtonParameters>& buttonParameters, const function<void(ButtonList*, int)>& cb)
	: callback(cb), FlexList<TextButton>(FlexList<TextButton>::Direction::Vertical)
{
	addEmptyPlaceholder(listWidth, buttonHeight, false);

	for (const ButtonParameters& params : buttonParameters)
	{
		addButton(params.id, params.text, buttonHeight, false);
	}

	setScrollOnDragEnabled(true);
	setSize(listWidth, jmax(buttonHeight, getIdealSize()));
}

ButtonList::~ButtonList()
{
}

ButtonList* ButtonList::fitToHeight(int maxHeight)
{
	if (getHeight() > maxHeight)
		setSize(getWidth(), maxHeight);
	
	return this;
}

void ButtonList::mouseUp(const MouseEvent& ev)
{
	if (ev.mods.isLeftButtonDown() && !ev.source.hasMouseMovedSignificantlySincePressed())
	{
		if (ButtonWithId* btn = dynamic_cast<ButtonWithId*>(ev.eventComponent))
		{
			callback(this, static_cast<ButtonWithId*>(btn)->id);
		}
		else
		{
			jassertfalse;
		}
	}
}

void ButtonList::addButton(int buttonId, const String& buttonText, int buttonHeight, bool refresh)
{
	removeEmptyPlaceholder(false);

	ButtonWithId* btn = new ButtonWithId(buttonId, buttonText);
	btn->addMouseListener(this, false);

	addItem(btn, buttonHeight, refresh);
}