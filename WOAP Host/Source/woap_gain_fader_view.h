#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_parameter.h"

namespace WOAP
{
	class Track;
	class TrackSendNode;

	namespace GUI
	{
		class GainFader;
		class TrackLevelIndicatorView;
		class VerticalDbScale;

		class GainFaderView : public Component, public Slider::Listener, public Parameter::Listener
		{
		public:
			GainFaderView(Track* tr = nullptr);
			virtual ~GainFaderView();

			virtual void resized() override;

			void setTrack(Track* tr);

			virtual void sliderValueChanged(Slider* slider) override;
			virtual void sliderDragStarted(Slider*) override;
			virtual void sliderDragEnded(Slider*) override;

			virtual void parameterChanged(Parameter*) override;

		protected:
			bool dragging = false;
			Track* track;
			ScopedPointer<GainFader> fader;
			ScopedPointer<TrackLevelIndicatorView> levelIndicator;
			ScopedPointer<VerticalDbScale> dbScale;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainFaderView)
		};

		class TrackSendNodeGainFaderView : public Component, public Slider::Listener, public Parameter::Listener
		{
		public:
			TrackSendNodeGainFaderView(TrackSendNode* trackSend = nullptr);
			virtual ~TrackSendNodeGainFaderView();

			virtual void resized() override;

			void setTrackSendNode(TrackSendNode* trackSend);

			virtual void sliderValueChanged(Slider* slider) override;
			virtual void sliderDragStarted(Slider*) override;
			virtual void sliderDragEnded(Slider*) override;

			virtual void parameterChanged(Parameter*) override;

		protected:
			bool dragging = false;
			TrackSendNode* trackSendNode;
			ScopedPointer<GainFader> fader;
			ScopedPointer<VerticalDbScale> dbScale;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackSendNodeGainFaderView)
		};
	}
}
