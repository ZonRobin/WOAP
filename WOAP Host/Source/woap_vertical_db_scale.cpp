#include "woap_vertical_db_scale.h"

using namespace WOAP;
using namespace WOAP::GUI;

VerticalDbScale::VerticalDbScale()
{
}

VerticalDbScale::~VerticalDbScale()
{
}

void VerticalDbScale::paint(Graphics& g)
{
	const Rectangle<int> bounds = getLocalBounds();
	const float range = valueMax - valueMin;
	const int scaleHeight = bounds.getHeight() - margin * 2;
	const int width = bounds.getWidth();

	g.setColour(Colour(20, 20, 20));

	for (int level : {10, 0, -10, -30, -50, -80})
	{
		const float levelRelative = (level - valueMin) / range;
		const int levelHeight = (int)(scaleHeight * std::pow(levelRelative, skewFactor));

		g.fillRect(0, scaleHeight + margin - levelHeight, width - 25, 1);
		g.drawText(String(level), 0, scaleHeight + margin - levelHeight - 8, width, 15, Justification::right, false);
	}

	for (int level : {5, -5, -20, -40, -60, -110})
	{
		const float levelRelative = (level - valueMin) / range;
		const int levelHeight = (int)(scaleHeight * std::pow(levelRelative, skewFactor));

		g.fillRect(0, scaleHeight + margin - levelHeight, width, 1);
	}
}
