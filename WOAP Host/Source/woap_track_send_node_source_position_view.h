#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_self_deletable.h"
#include "woap_track_send_node.h"

namespace WOAP
{
	class Track;

	namespace GUI
	{
		class TrackSendNodeSourcePositionView : public TextButton, public TrackSendNode::AudioSourcePositionListener
		{
		public:
			TrackSendNodeSourcePositionView(Track* tr, TrackSendNode* sendNode);
			~TrackSendNodeSourcePositionView();

			void updateText();
			virtual void clicked() override;

			virtual void sendAudioSourcePositionChanged(TrackSendNode* send) override;
			virtual void aboutToDelete(TrackSendNode*) override {}

		protected:
			Track* track;
			TrackSendNode* trackSendNode;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSendNodeSourcePositionView)
		};
	}
}