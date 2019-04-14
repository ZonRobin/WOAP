#include "woap_icon_button.h"

using namespace WOAP;
using namespace WOAP::GUI;

IconButton::IconButton(const Icon& iconToUse)
	: Button(String()), iconPainted(iconToUse)
{
	setMouseCursor(MouseCursor::PointingHandCursor);
}

IconButton::~IconButton()
{
}

void IconButton::paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
	if (showBackground || isButtonDown)
	{
		getLookAndFeel().drawButtonBackground(
			g,
			*this,
			findColour(getToggleState() ? TextButton::buttonOnColourId : TextButton::buttonColourId),
			isMouseOverButton,
			isButtonDown
		);
	}

	const float glyphSize = jmin(getWidth(), getHeight()) * 0.66f;
	FontAwesome::drawCenterd(
		g,
		iconPainted,
		glyphSize,
		findColour(getToggleState() ? TextButton::textColourOnId : TextButton::textColourOffId).withAlpha(isEnabled() ? 1 : 0.6f),
		getLocalBounds().withTrimmedTop(2)
	);
}