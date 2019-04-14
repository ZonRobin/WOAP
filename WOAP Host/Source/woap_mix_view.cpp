#include "woap_mix_view.h"
#include "woap_track_view.h"
#include "woap_track.h"
#include "woap_main_view.h"
#include "woap_audio_core_view.h"

using namespace WOAP;
using namespace WOAP::GUI;

MixView::MixView()
{
	trackList = new FlexList<TrackView>(FlexList<TrackView>::Direction::Horizontal);
	trackList->setScrollOnDragEnabled(true);
	trackList->addListener(this);
	trackList->setSwapGetsureEnabled(true);
	addAndMakeVisible(trackList);

	AudioCore::getInstance()->addListener(this);
	for (Track* track : AudioCore::getInstance()->getTracks())
	{
		addTrack(track, -1, false);
	}
}

MixView::~MixView()
{
	AudioCore::getInstance()->removeListener(this);
}

void MixView::resized()
{
	trackList->setBounds(getLocalBounds());
}

void MixView::trackAdded(Track* newTrack, int position)
{
	addTrack(newTrack, position);
}

void MixView::trackRemoved(Track* oldTrack)
{
	removeTrack(oldTrack);
}

void MixView::trackMoved(Track*, int oldPosition, int newPosition)
{
	trackList->moveItem(oldPosition, newPosition);
}

void MixView::addTrack(Track* newTrack, int position, bool repaint)
{
	TrackView* trackView = new TrackView(newTrack);
	trackView->addListener(this);
	trackList->addItem(trackView, 100, position, repaint);
}

void MixView::removeTrack(Track* oldTrack)
{
	for (TrackView* item : trackList->getItems())
	{
		if (item->getTrack() == oldTrack)
		{
			trackList->removeItem(item);
			return;
		}
	}
}

void MixView::setSelectedTrack(Track* tr)
{
	for (TrackView* item : trackList->getItems())
		item->setSelected(item->getTrack() == tr);
}

void MixView::removeTrackRequested(TrackView* view)
{
	AudioCore::getInstance()->removeTrack(view->getTrack());
}

void MixView::swapGestureCallback(FlexList<TrackView>*, int sourceIndex, int targetIndex)
{
	AudioCore::getInstance()->moveTrack(sourceIndex, targetIndex);
}

void MixView::clickedToEmptySpace(FlexList<TrackView>*, const MouseEvent &)
{
	MainView::getInstance()->getAudioCoreView()->setSelectedTrack(nullptr);
}