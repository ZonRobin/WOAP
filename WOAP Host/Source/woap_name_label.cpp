#include "woap_name_label.h"
#include "woap_look_and_feel.h"

using namespace WOAP;
using namespace WOAP::GUI;

NameLabel::NameLabel()
	: maxInputLength(12)
{
	setJustificationType(Justification::centred);	
	setMinimumHorizontalScale(0.1f);
}

TextEditor* NameLabel::createEditorComponent()
{
	TextEditor* const ed = Label::createEditorComponent();
	ed->setJustification(Justification::centred);
	ed->setInputRestrictions(maxInputLength);

	return ed;
}

void NameLabel::setFontSize(float newSize)
{
	setFont(getFont().withHeight(newSize));
}

void NameLabel::setBold(bool bold)
{
	Font f = getFont();
	f.setBold(bold);
	setFont(f);
}

void NameLabel::setMaxInputLength(int maxLength)
{
	maxInputLength = maxLength;
}

void NameLabel::setBackgroundColour(Colour newColour)
{
	setBackgroundColour(newColour, newColour.getPerceivedBrightness() > 0.5f);
}

void NameLabel::setBackgroundColour(Colour newColour, bool useDarkColour)
{
	setColour(Label::ColourIds::backgroundColourId, newColour);

	if (useDarkColour)
	{
		setColour(Label::ColourIds::textColourId, CustomColours::darkTextColour);
		setColour(Label::ColourIds::textWhenEditingColourId, CustomColours::darkTextColour);
	}
	else
	{
		setColour(Label::ColourIds::textColourId, CustomColours::lightTextColour);
		setColour(Label::ColourIds::textWhenEditingColourId, CustomColours::lightTextColour);
	}

	repaint();
}

MixerNodeNameLabel::MixerNodeNameLabel(bool allowEdit, MixerNode* node)
{
	setBold(true);
	setFontSize(14.0f);

	if (allowEdit)
		setEditable(false, true);

	setMixerNode(node);
}

MixerNodeNameLabel::~MixerNodeNameLabel()
{
	setMixerNode(nullptr);
}

void MixerNodeNameLabel::setMixerNode(MixerNode* node)
{
	if (mixerNode != nullptr)
	{
		mixerNode->removeListener(this);
	}

	mixerNode = node;

	if (node != nullptr)
	{
		node->addListener(this);
		setText(node->getName(), NotificationType::dontSendNotification);
	}
	else
	{
		setText("", NotificationType::dontSendNotification);
	}
	
	updateColours();
}

void MixerNodeNameLabel::nameChanged(MixerNode*)
{
	setText(mixerNode->getName(), NotificationType::dontSendNotification);
}

void MixerNodeNameLabel::colourChanged(MixerNode*)
{
	updateColours();
}

void MixerNodeNameLabel::aboutToDelete(MixerNode*)
{
	setMixerNode(nullptr);
}

void MixerNodeNameLabel::textWasChanged()
{
	if (mixerNode == nullptr)
		return;

	const String newName = getText();

	if (newName != mixerNode->getName())
		mixerNode->setName(newName);
}

void MixerNodeNameLabel::updateColours()
{
	if (mixerNode == nullptr)
		return;

	setBackgroundColour(mixerNode->getColour());
}