#include "woap_solo_bus_settings_component.h"
#include "woap_mixer_node_selector.h"
#include "woap_solo_bus.h"
#include "woap_level_indicator.h"
#include "woap_simple_call_out_box.h"

using namespace WOAP;
using namespace WOAP::GUI;

SoloBusSettingsComponent::SoloBusSettingsComponent()
{
	soloBus = AudioCore::getInstance()->getSoloBus();

	selectorButton = new TextButton("Output");
	selectorButton->addListener(this);
	selectorButton->setMouseCursor(MouseCursor::PointingHandCursor);
	addAndMakeVisible(selectorButton);

	levelIndicator = new LevelIndicator();
	addAndMakeVisible(levelIndicator);

	startTimerHz(25);
}

SoloBusSettingsComponent::~SoloBusSettingsComponent()
{
	SimpleCallOutBox::close(selectorCallOut);
}

void SoloBusSettingsComponent::resized()
{
	Rectangle<int> bounds = getLocalBounds();

	selectorButton->setBounds(0, 5, 80, 30);
	levelIndicator->setBounds(200, 0, 15, getHeight());
}

void SoloBusSettingsComponent::paint(Graphics& g)
{
	g.drawFittedText("Output level", 90, 5, 100, 30, Justification::centred, 1);
}

void SoloBusSettingsComponent::timerCallback()
{
	const Array<float>& signallevels = soloBus->getSignalLevels();
	levelIndicator->setValues(signallevels[0], signallevels[1]);
}

void SoloBusSettingsComponent::buttonClicked(Button*)
{
	selectorCallOut = SimpleCallOutBox::open(new MixerOutputNodeSelector(120, soloBus), getScreenBounds());
}
