#include "woap_track_plugin_list_element_view.h"
#include "woap_track.h"
#include "woap_main_view.h"
#include "woap_audio_core_view.h"
#include "woap_drag_handle.h"
#include "woap_look_and_feel.h"
#include "woap_name_label.h"

using namespace WOAP;
using namespace WOAP::GUI;

TrackPluginListElementView::TrackPluginListElementView(bool simpleElement_, AudioProcessorPlayerDouble* pl, Track* tr)
	: simpleElement(simpleElement_), track(tr), plugin(pl)
{
	setMouseCursor(MouseCursor::PointingHandCursor);

	if (!simpleElement)
	{
		dragHandle = new DragHandle();
		dragHandle->addMouseListener(this, true);
		addAndMakeVisible(dragHandle);
	}
	
	pluginNameLabel = new NameLabel();
	pluginNameLabel->setText(plugin->getName(), NotificationType::dontSendNotification);
	pluginNameLabel->addMouseListener(this, true);
	pluginNameLabel->setBackgroundMargin(1, 1);
	pluginNameLabel->setMouseCursor(MouseCursor::PointingHandCursor);
	addAndMakeVisible(pluginNameLabel);

	plugin->addListener(this);
	plugin->getBypassParameter()->addListener(this);
	plugin->getActiveStateParameter()->addListener(this);
	track->addListener(this);

	updateColours();
}

TrackPluginListElementView::~TrackPluginListElementView()
{
	track->removeListener(this);
	plugin->removeListener(this);
	plugin->getBypassParameter()->removeListener(this);
	plugin->getActiveStateParameter()->removeListener(this);
}

void TrackPluginListElementView::selfDelete() const
{
	MessageManager::callAsync([this]() {
		track->removePlugin(plugin);
	});
}

void TrackPluginListElementView::resized()
{
	if (!simpleElement)
	{
		pluginNameLabel->setBounds(getLocalBounds().withWidth(getWidth() - getHeight() - 5));
		dragHandle->setBounds(getWidth() - getHeight() - 4, 4, getHeight() - 6, getHeight() - 6);
	}
	else
	{
		pluginNameLabel->setBounds(getLocalBounds());
	}
}

void TrackPluginListElementView::paint(Graphics& g)
{
	g.setColour(pluginIsEnabled() ? track->getColour().brighter(0.3f) : Colour(160, 160, 160));
	g.fillRoundedRectangle(getLocalBounds().reduced(1, 1).toFloat(), 3.0f);
}

void TrackPluginListElementView::updateColours()
{
	pluginNameLabel->setBackgroundColour(
		Colours::transparentBlack,
		pluginIsEnabled() ? track->getColour().getPerceivedBrightness() > 0.5f : true
	);

	repaint();
}

void TrackPluginListElementView::openEditor() const
{
	MainView::getInstance()->getAudioCoreView()->openPluginEditor(plugin);
}

void TrackPluginListElementView::nameChanged(AudioProcessorPlayerDouble*)
{
	pluginNameLabel->setText(plugin->getName(), NotificationType::dontSendNotification);
}

void TrackPluginListElementView::mouseUp(const MouseEvent& event)
{
	if (event.mods.isLeftButtonDown() && !event.source.hasMouseMovedSignificantlySincePressed())
	{
		openEditor();
	}
}

void TrackPluginListElementView::mouseDown(const MouseEvent& ev)
{
	if (ev.eventComponent == dragHandle)
	{
		MainView::getInstance()->startDragging(var(), this);
	}
}