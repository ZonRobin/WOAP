#include "woap_loading_spinner.h"
#include "woap_look_and_feel.h"

using namespace WOAP;
using namespace WOAP::GUI;

LoadingSpinner::LoadingSpinner(bool d) 
	: dark(d)
{
	visibilityChanged();
}

LoadingSpinner::~LoadingSpinner()
{
}

void LoadingSpinner::paint(Graphics& g)
{
	if (dark)
		getLookAndFeel().drawSpinningWaitAnimation(g, CustomColours::black, 0, 0, getWidth(), getHeight());
	else
		getLookAndFeel().drawSpinningWaitAnimation(g, CustomColours::grey, 0, 0, getWidth(), getHeight());
}

void LoadingSpinner::timerCallback()
{
	repaint();
}

void LoadingSpinner::visibilityChanged()
{
	if (isVisible())
		startTimerHz(40);
	else
		stopTimer();
}
