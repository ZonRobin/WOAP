#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_window.h"
#include "woap_audio_core.h"

namespace WOAP
{
	class AudioCore;
	class AudioProcessorPlayerDouble;

	namespace GUI
	{
		class MixView;
		class TrackDetailView;
		class PluginEditorView;
	
		class AudioCoreView : public Component, public Window::CloseWindowListener, public AudioCore::Listener
		{
		public:
			AudioCoreView();
			virtual ~AudioCoreView();

			void resized() override;
			void openPluginEditor(AudioProcessorPlayerDouble* plugin);
			int getXPositionForNewPlugin(const Array<int>& posXOfPlugins, const PluginEditorView* editor) const;
			int getYPositionForNewPlugin(const PluginEditorView* editor) const;

			virtual void closeWindowTriggered(Window* closedWindow) override;

			virtual void trackRemoved(Track*) override;
			virtual void aboutToDelete(AudioCore*) override {}

			void setSelectedTrack(Track* track);
			Track* getSelectedTrack() const { return selectedTrack; }

			int getSelectedTrackIndex() const;
			int getIndexAfterSelectedTrack() const;

			virtual void mouseUp(const MouseEvent& event) override;

		protected:
			int mixViewHeight;
			ScopedPointer<MixView> mixView;
			ScopedPointer<TrackDetailView> trackDetailView;
			OwnedArray<PluginEditorView> pluginEditors;

			Track* selectedTrack = nullptr;
		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioCoreView)
		};
	}
}
