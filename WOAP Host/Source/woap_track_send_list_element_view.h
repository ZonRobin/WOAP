#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_self_deletable.h"
#include "woap_mixer_node.h"

namespace WOAP
{
	class Track;
	class TrackSendNode;

	namespace GUI
	{
		class DragHandle;
		class FaderView;
		class SimpleCallOutBox;
		class NameLabel;

		class TrackSendListElementView : public Component, public SelfDeletable, public MixerNode::Listener
		{
		public:
			TrackSendListElementView(TrackSendNode* send, Track* sendOwner);
			virtual ~TrackSendListElementView();

			virtual void selfDelete() const override;

			TrackSendNode* getTrackSendNode() const { return trackSendNode; }
			Track* getTrack() const { return track; }

			void resized() override;
			void paint(Graphics& g) override;

			void updateColours();

			virtual void mouseUp(const MouseEvent& event) override;
			virtual void mouseDown(const MouseEvent& event) override;

			virtual void nameChanged(MixerNode* node) override;
			virtual void colourChanged(MixerNode*) override { updateColours(); }
			virtual void aboutToDelete(MixerNode*) override {}

		protected:
			Track* track;
			TrackSendNode* trackSendNode;
			WeakReference<SimpleCallOutBox> sendDetailCallOut;

			ScopedPointer<NameLabel> sendNameLabel;
			ScopedPointer<DragHandle> dragHandle;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSendListElementView)
		};
	}
}
