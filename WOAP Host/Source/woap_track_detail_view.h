#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_track.h"

namespace WOAP
{
	namespace GUI
	{
		class MixerNodeNameLabel;
		class TrackColourSelector;
		class TrackPluginListView;
		class TrackSendListView;
		class MixerInputNodeSelector;
		class MixerOutputNodeSelector;
		class TrackIOSelector;
		class MuteButtonView;
		class SoloButtonView;
		class PanView;
		class GainFaderView;

		class TrackDetailView : public Component, public Track::Listener
		{
		public:
			TrackDetailView();
			virtual ~TrackDetailView();

			void paint(Graphics& g) override;
			void resized() override;

			void setTrack(Track* tr);
			void trackChanged();

			virtual void nameChanged(MixerNode*) override;
			virtual void colourChanged(MixerNode*) override;
			virtual void aboutToDelete(MixerNode*) override {}

		protected:
			Track* track;

			ScopedPointer<MixerNodeNameLabel> trackNameLabel;
			ScopedPointer<TrackColourSelector> trackColourSelector;
			ScopedPointer<TrackPluginListView> trackPluginListView;
			ScopedPointer<TrackSendListView> trackSendListView;
			ScopedPointer<TrackIOSelector> inputNodeSelector;
			ScopedPointer<TrackIOSelector> outputNodeSelector;

			ScopedPointer<MuteButtonView> muteButton;
			ScopedPointer<SoloButtonView> soloButton;
			ScopedPointer<PanView> panFader;
			ScopedPointer<GainFaderView> gainFader;
		};
	}
}
