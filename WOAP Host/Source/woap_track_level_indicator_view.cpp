#include "woap_track_level_indicator_view.h"
#include "woap_level_indicator.h"
#include "woap_track.h"

using namespace WOAP;
using namespace WOAP::GUI;

TrackLevelIndicatorView::TrackLevelIndicatorView(Track* tr)
{
	setTrack(tr);
	setAveraging(0.1f);
}

TrackLevelIndicatorView::~TrackLevelIndicatorView()
{
}

void TrackLevelIndicatorView::setTrack(Track* tr)
{
	track = tr;

	if (tr != nullptr)
	{
		const Array<float>& signalLevels = track->getSignalLevels();
		setValuesNoAverage(
			signalLevels[0],
			signalLevels[1],
			Time::getApproximateMillisecondCounter() - track->getLastClipTimeMilliseconds() <= 300,
			track->areSignalLevelsInSoloMode()
		);
		startTimer(1, 10);
	}
	else
	{
		stopTimer(1);
	}
}

void TrackLevelIndicatorView::timerCallback(int timerID)
{
	if (timerID == 1)
	{
		if (track == nullptr)
			return;

		const Array<float>& signalLevels = track->getSignalLevels();
		setValues(
			signalLevels[0],
			signalLevels[1],
			Time::getApproximateMillisecondCounter() - track->getLastClipTimeMilliseconds() <= 300,
			track->areSignalLevelsInSoloMode()
		);
	}
	else
	{
		LevelIndicator::timerCallback(timerID);
	}
}