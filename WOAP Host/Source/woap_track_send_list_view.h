#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_track.h"
#include "woap_flex_list.h"
#include "woap_track_send_list_element_view.h"

namespace WOAP
{
	class TrackSendNode;

	namespace GUI
	{
		class IconButton;

		class TrackSendListView : public Component, public Track::SendsListener, public Button::Listener, public FlexList<TrackSendListElementView>::Listener
		{
		public:
			TrackSendListView(Track* track = nullptr);
			virtual ~TrackSendListView();

			void paint(Graphics& g) override;
			void resized() override;

			void setTrack(Track* newTrack);

			void addSend(TrackSendNode* send, int position = -1);
			void removeSend(TrackSendNode* send);

			virtual void buttonClicked(Button*) override;

			virtual void sendAdded(TrackSendNode* send, int position) override;
			virtual void sendRemoved(TrackSendNode* send) override;
			virtual void sendMoved(TrackSendNode* send, int oldPosition, int newPosition) override;
			virtual void aboutToDelete(Track*) override {}

			virtual void swapGestureCallback(FlexList<TrackSendListElementView>* list, int sourceIndex, int targetIndex) override;

		protected:
			Track* track;
			ScopedPointer<FlexList<TrackSendListElementView>> sendList;
			ScopedPointer<IconButton> addSendButton;
			ScopedPointer<Label> headingLabel;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSendListView)
		};
	}
}

