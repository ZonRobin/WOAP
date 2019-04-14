#include "woap_track_plugin_list_view.h"
#include "woap_audio_processor_player_double.h"
#include "woap_plugin_manager.h"
#include "woap_simple_call_out_box.h"
#include "woap_button_list.h"
#include "woap_icon_button.h"

using namespace WOAP;
using namespace WOAP::GUI;

TrackPluginListView::TrackPluginListView(bool showSimpleList, Track* tr)
	: simpleList(showSimpleList)
{
	pluginList = new FlexList<TrackPluginListElementView>(FlexList<TrackPluginListElementView>::Direction::Vertical);
	pluginList->setScrollOnDragEnabled(true);
	pluginList->setSwapGetsureEnabled(true);
	pluginList->addListener(this);
	pluginList->setAutoScrollSpeed(1);
	pluginList->setAutoScrollDistance(40);
	addAndMakeVisible(pluginList);

	if (!simpleList)
	{
		addPluginButton = new IconButton(FontAwesome_Plus);
		addPluginButton->addListener(this);
		addPluginButton->setShowBackground(false);
		addAndMakeVisible(addPluginButton);

		headingLabel = new Label("Heading", "Plugins");
		addAndMakeVisible(headingLabel);
	}
	
	setTrack(tr);
}

TrackPluginListView::~TrackPluginListView()
{
	setTrack(nullptr);
}

void TrackPluginListView::setTrack(Track* tr)
{
	if (track != nullptr)
	{
		track->removePluginsListener(this);
		pluginList->removeAllItems();
	}

	track = tr;

	if (tr != nullptr)
	{
		tr->addPluginsListener(this);
		for (AudioProcessorPlayerDouble* plugin : tr->getPlugins())
		{
			addPlugin(plugin);
		}
	}
}

void TrackPluginListView::paint(Graphics& g)
{
	if (simpleList)
	{
		g.setColour(Colour(215, 215, 215));
		g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.0f);
	}
	else
	{
		Path head;
		head.addRoundedRectangle(0, 0, (float)getWidth(), 35.0f, 3.0f, 3.0f, true, true, false, false);
		g.setColour(Colour(60, 60, 60));
		g.fillPath(head);

		Path fill;
		fill.addRoundedRectangle(0, 35.0f, (float)getWidth(), (float)getHeight() - 35.0f, 3.0f, 3.0f, false, false, true, true);
		g.setColour(Colour(215, 215, 215));
		g.fillPath(fill);
	}
}

void TrackPluginListView::resized()
{
	if (!simpleList)
	{
		pluginList->setBounds(getLocalBounds().withTrimmedTop(35));

		headingLabel->setBounds(10, 2, 100, 30);
		addPluginButton->setBounds(getWidth() - 45, 5, 40, 26);
	}
	else
	{
		pluginList->setBounds(getLocalBounds());
	}
}

void TrackPluginListView::addPlugin(AudioProcessorPlayerDouble* plugin, int position)
{
	TrackPluginListElementView* element = new TrackPluginListElementView(simpleList, plugin, track);
	pluginList->addItem(element, 35, position, true);

	if (plugin->getTypeId() == idOfPluginToOpenEditor)
		element->openEditor();

	idOfPluginToOpenEditor = -1;
}

void TrackPluginListView::removePlugin(AudioProcessorPlayerDouble* plugin)
{
	for (TrackPluginListElementView* pluginElementView : pluginList->getItems())
	{
		if (pluginElementView->getPlugin() == plugin)
		{
			pluginList->removeItem(pluginElementView);
			return;
		}
	}
}

void TrackPluginListView::pluginAdded(AudioProcessorPlayerDouble* plugin, int position)
{
	addPlugin(plugin, position);
}

void TrackPluginListView::pluginRemoved(AudioProcessorPlayerDouble* plugin)
{
	removePlugin(plugin);
}

void TrackPluginListView::pluginMoved(AudioProcessorPlayerDouble*, int oldPosition, int newPosition)
{
	pluginList->moveItem(oldPosition, newPosition);
}

void TrackPluginListView::buttonClicked(Button* btn)
{
	if (track == nullptr)
		return;

	if (btn == addPluginButton)
	{
		showPluginListMenu();
	}
}

void TrackPluginListView::swapGestureCallback(FlexList<TrackPluginListElementView>*, int sourceIndex, int targetIndex)
{
	track->movePlugin(sourceIndex, targetIndex);
}

void TrackPluginListView::clickedToEmptySpace(FlexList<TrackPluginListElementView>*, const MouseEvent& ev)
{
	if (simpleList && ev.getNumberOfClicks() == 2)
		showPluginListMenu();
}

void TrackPluginListView::showPluginListMenu()
{
	Array<ButtonList::ButtonParameters> buttons;
	for (PluginDescription* pluginDescription : PluginManager::getInstance()->getKnownPluginList())
	{
		buttons.add({ pluginDescription->uid, pluginDescription->name });
	}

	SimpleCallOutBox::open(new ButtonList(40, 250, buttons, [this](ButtonList* list, int pluginId) {
		idOfPluginToOpenEditor = pluginId;
		track->addPluginByUid(pluginId);
		list->getParentComponent()->exitModalState(0);
	}), simpleList ? getScreenBounds() : addPluginButton->getScreenBounds());
}
