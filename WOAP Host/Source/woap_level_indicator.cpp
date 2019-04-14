#include "woap_level_indicator.h"
#include "woap_look_and_feel.h"

using namespace WOAP;
using namespace WOAP::GUI;

LevelIndicator::LevelIndicator()
{
	startTimer(0, 33);
}

LevelIndicator::~LevelIndicator()
{

}

void LevelIndicator::paint(Graphics& g)
{
	const Rectangle<float> bounds = getLocalBounds().toFloat();
	const float width = bounds.getWidth();
	const float height = bounds.getHeight();

	const float leftHeight = levelToHeight(values[0]);
	const float rightHeight = levelToHeight(values[1]);

	g.setColour(findColour(ResizableWindow::backgroundColourId));
	g.fillRoundedRectangle(bounds, 3.0f);

	Path levels;
	levels.addRoundedRectangle(0, height - leftHeight, width / 2.0f, leftHeight, 3.0f, 3.0f, true, false, true, false);
	levels.addRoundedRectangle(width / 2.0f, height - rightHeight, width / 2, rightHeight, 3.0f, 3.0f, false, true, false, true);

	g.setColour(clipping ? CustomColours::red : (soloed ? CustomColours::yellow : CustomColours::green));
	g.fillPath(levels);

	if (clipping)
		g.drawRoundedRectangle(0, 0, width, height, 3.0f, 3.0f);
}

void LevelIndicator::setValues(float valueL, float valueR, bool clip, bool solo)
{
	if (soloed != solo)
	{
		values[0] = valueL;
		values[1] = valueR;
	}
	else
	{
		values[0] = valueL * averaging + values[0] * (1 - averaging);
		values[1] = valueR * averaging + values[1] * (1 - averaging);
	}

	clipping = clip;
	soloed = solo;
}

void LevelIndicator::setValuesNoAverage(float valueL, float valueR, bool clip, bool solo)
{
	values[0] = valueL;
	values[1] = valueR;

	clipping = clip;
	soloed = solo;
}

float LevelIndicator::levelToHeight(float gain) const
{
	const float dbVal = Decibels::gainToDecibels(gain, valueMin);
	const float relative = (dbVal - valueMin) / (valueMax - valueMin);
	return getHeight() * jlimit(0.0f, 1.0f, std::pow(relative, skewFactor));
}

void LevelIndicator::timerCallback(int timerID)
{
	if (timerID == 0)
		repaint();
}
