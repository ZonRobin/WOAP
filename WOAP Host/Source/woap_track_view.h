#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_self_deletable.h"
#include "woap_mixer_node.h"

namespace WOAP
{
	class Track;

	namespace GUI
	{
		class TrackPluginListView;
		class MixerNodeSelector;
		class PanView;
		class TrackSendListView;
		class GainFaderView;
		class MixerNodeNameLabel;
		class ButtonView;
		class DragHandle;
		
		class TrackView : public Component, public SelfDeletable, public MixerNode::Listener
		{
		public:
			class Listener
			{
			public:
				virtual void removeTrackRequested(TrackView* view) = 0;
			};

			TrackView(Track* track);
			virtual ~TrackView();

			virtual void selfDelete() const override;

			void paint(Graphics& g) override;

			virtual void mouseDown(const MouseEvent& event) override;
			virtual void mouseDrag(const MouseEvent& event) override;
			virtual void mouseUp(const MouseEvent& event) override;

			Track* getTrack() const { return track; }
			void setSelected(bool selected);

			void addListener(TrackView::Listener* listener) { listeners.add(listener); }
			void removeListener(TrackView::Listener* listener) { listeners.remove(listener); }

			virtual void nameChanged(MixerNode*) {}
			virtual void colourChanged(MixerNode*) { repaint(); }
			virtual void aboutToDelete(MixerNode*) {}

		protected:
			Track* track;
			bool isSelected = false;
			bool startDrag = false;

			ScopedPointer<TrackPluginListView> pluginListView;
			ScopedPointer<MixerNodeNameLabel> nameLabel;

			ScopedPointer<ButtonView> muteButton;
			ScopedPointer<ButtonView> soloButton;
			ScopedPointer<PanView> panFader;
			ScopedPointer<GainFaderView> gainFader;

			ScopedPointer<DragHandle> dragHandle;

			ListenerList<TrackView::Listener> listeners;
		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackView)
		};
	}
}

