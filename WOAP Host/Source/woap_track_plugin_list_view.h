#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_track.h"
#include "woap_flex_list.h"
#include "woap_track_plugin_list_element_view.h"

namespace WOAP
{
	class AudioProcessorPlayerDouble;

	namespace GUI
	{
		class IconButton;

		class TrackPluginListView : public Component, public Track::PluginsListener, public TextButton::Listener, public FlexList<TrackPluginListElementView>::Listener
		{
		public:
			TrackPluginListView(bool showSimpleList, Track* track = nullptr);
			virtual ~TrackPluginListView();
			
			void setTrack(Track* newTrack);

			void paint(Graphics& g) override;
			void resized() override;

			void addPlugin(AudioProcessorPlayerDouble* plugin, int position = -1);
			void removePlugin(AudioProcessorPlayerDouble* plugin);

			virtual void pluginAdded(AudioProcessorPlayerDouble* plugin, int position) override;
			virtual void pluginRemoved(AudioProcessorPlayerDouble* plugin) override;
			virtual void pluginMoved(AudioProcessorPlayerDouble* plugin, int oldPosition, int newPosition) override;
			virtual void aboutToDelete(Track*) override {}

			virtual void buttonClicked(Button*) override;

			virtual void swapGestureCallback(FlexList<TrackPluginListElementView>* list, int sourceIndex, int targetIndex) override;
			virtual void clickedToEmptySpace(FlexList<TrackPluginListElementView>*, const MouseEvent&) override;

			void showPluginListMenu();

		protected:
			bool simpleList = false;
			Track* track = nullptr;
			int idOfPluginToOpenEditor = -1;

			ScopedPointer<FlexList<TrackPluginListElementView>> pluginList;
			ScopedPointer<IconButton> addPluginButton;
			ScopedPointer<Label> headingLabel;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPluginListView)
		};
	}
}
