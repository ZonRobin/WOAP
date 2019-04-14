#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	class Track;
	class TrackSendNode;

	namespace GUI
	{
		class PanView;
		class MixerNodeNameLabel;
		class TrackSendNodeGainFaderView;
		class TrackSendNodeSourcePositionView;

		class TrackSendNodeDetailView : public Component
		{
		public:
			TrackSendNodeDetailView(TrackSendNode* send, Track* sendOwner);
			virtual ~TrackSendNodeDetailView();

			TrackSendNode* getTrackSendNode() const { return trackSendNode; }
			Track* getTrack() const { return track; }

			void paint(Graphics& g) override;
			virtual void resized() override;

		protected:
			Track* track;
			TrackSendNode* trackSendNode;

			ScopedPointer<MixerNodeNameLabel> nameLabel;
			ScopedPointer<TrackSendNodeGainFaderView> gainFaderView;
			ScopedPointer<PanView> panFaderView;
			ScopedPointer<TrackSendNodeSourcePositionView> audioSourcePositionView;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSendNodeDetailView)
		};
	}
}
