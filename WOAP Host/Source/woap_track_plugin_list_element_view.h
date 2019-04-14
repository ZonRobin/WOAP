#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_audio_processor_player_double.h"
#include "woap_parameter_bool.h"
#include "woap_self_deletable.h"
#include "woap_mixer_node.h"

namespace WOAP
{
	class Track;

	namespace GUI
	{
		class DragHandle;
		class NameLabel;

		class TrackPluginListElementView : public Component, public AudioProcessorPlayerDouble::Listener, public SelfDeletable, public MixerNode::Listener, public Parameter::Listener
		{
		public:
			TrackPluginListElementView(bool simpleElement, AudioProcessorPlayerDouble* plugin, Track* pluginOwner);
			virtual ~TrackPluginListElementView();

			virtual void selfDelete() const override;

			void resized() override;
			void paint(Graphics& g) override;

			void updateColours();

			AudioProcessorPlayerDouble* getPlugin() const { return plugin; }
			Track* getTrack() const { return track; }

			bool pluginIsEnabled() const { return !*plugin->getBypassParameter() && *plugin->getActiveStateParameter(); }
			void openEditor() const;

			virtual void nameChanged(AudioProcessorPlayerDouble*) override;
			virtual void aboutToDelete(AudioProcessorPlayerDouble*) override {}

			virtual void nameChanged(MixerNode*) override {}
			virtual void colourChanged(MixerNode*) override { updateColours(); }
			virtual void parameterChanged(Parameter*) override { updateColours(); }
			virtual void aboutToDelete(MixerNode*) override {}

			virtual void mouseUp(const MouseEvent& event) override;
			virtual void mouseDown(const MouseEvent& event) override;

		protected:
			bool simpleElement = true;
			Track* track;
			AudioProcessorPlayerDouble* plugin;

			ScopedPointer<NameLabel> pluginNameLabel;
			ScopedPointer<DragHandle> dragHandle;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPluginListElementView)
		};
	}
}
