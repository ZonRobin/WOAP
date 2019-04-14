#include "woap_loading_screen_component.h"

LoadingScreenComponent::LoadingScreenComponent()
{

}

LoadingScreenComponent::~LoadingScreenComponent()
{
}

void LoadingScreenComponent::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
	
	g.setColour(getLookAndFeel().findColour(Label::ColourIds::textColourId));
	g.setFont(25.0f);
	g.drawFittedText("Loading", getLocalBounds(), Justification::centred, 1);
}
