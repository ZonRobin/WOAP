#include "woap_audio_core_view.h"
#include "woap_mix_view.h"
#include "woap_track_detail_view.h"
#include "woap_audio_processor_player_double.h"
#include "woap_plugin_editor_view.h"

using namespace WOAP;
using namespace WOAP::GUI;

AudioCoreView::AudioCoreView()
{
	mixView = new MixView();
	addAndMakeVisible(mixView);

	trackDetailView = new TrackDetailView();
	addAndMakeVisible(trackDetailView);

	mixViewHeight = 590;

	AudioCore::getInstance()->addListener(this);
}

AudioCoreView::~AudioCoreView()
{
	AudioCore::getInstance()->removeListener(this);
}

void AudioCoreView::resized()
{
	const Rectangle<int> bounds = getLocalBounds();

	if (selectedTrack == nullptr)
	{
		trackDetailView->setVisible(false);
		mixView->setBounds(bounds.withTrimmedLeft(5).withTrimmedTop(getHeight() - mixViewHeight).withTrimmedBottom(5));
	}
	else
	{
		mixView->setBounds(bounds.withTrimmedRight(255).withTrimmedLeft(5).withTrimmedTop(getHeight() - mixViewHeight).withTrimmedBottom(5));
		trackDetailView->setVisible(true);
		trackDetailView->setBounds(bounds.withTrimmedLeft(bounds.getWidth() - 250).withTrimmedBottom(5));
	}
}

void AudioCoreView::openPluginEditor(AudioProcessorPlayerDouble* plugin)
{
	Array<int> pluginXPositions;
	pluginXPositions.ensureStorageAllocated(pluginEditors.size());
	
	for (PluginEditorView* editor : pluginEditors)
	{
		if (editor->getPlugin() == plugin)
		{
			editor->toFront(true);
			return;
		}

		pluginXPositions.addUsingDefaultSort(editor->getPosition().x);
	}
	
	PluginEditorView* editor = new PluginEditorView(this, plugin);
	pluginEditors.add(editor);

	editor->setTopLeftPosition(getXPositionForNewPlugin(pluginXPositions, editor), getYPositionForNewPlugin(editor));
	editor->setVisible(true);
	editor->setAlwaysOnTop(true);
}

int AudioCoreView::getXPositionForNewPlugin(const Array<int>& posXOfPlugins, const PluginEditorView* editor) const
{
	const int windowWidth = editor->getParentWidth();
	const int maxPosX = windowWidth - editor->getWidth();
	const int constrainedBlockWidth = 150;
	int previousPluginX = 0;

	if (posXOfPlugins.isEmpty())
		return maxPosX / 2;

	for (int posX : posXOfPlugins)
	{
		if (posX > previousPluginX + constrainedBlockWidth)
			return jmin((posX - previousPluginX) / 2 + previousPluginX, maxPosX);

		previousPluginX = posX;
	}

	if (posXOfPlugins.getFirst() > constrainedBlockWidth / 2)
		return 0;

	if (maxPosX > posXOfPlugins.getLast() + constrainedBlockWidth)
		return posXOfPlugins.getLast() + constrainedBlockWidth;

	return maxPosX / 2;
}

int AudioCoreView::getYPositionForNewPlugin(const PluginEditorView* editor) const
{
	const int areaHeight = getHeight() - mixViewHeight;
	const int editorHeight = editor->getHeight();
	
	if (editorHeight > areaHeight)
		return 20;

	return (areaHeight - editorHeight) / 2;
}

void AudioCoreView::closeWindowTriggered(Window* closedWindow)
{
	if (PluginEditorView* editorView = dynamic_cast<PluginEditorView*>(closedWindow))
		pluginEditors.removeObject(editorView, true);
	else
		jassertfalse;
}

void AudioCoreView::trackRemoved(Track* track)
{
	if (selectedTrack == track)
		setSelectedTrack(nullptr);
}

void AudioCoreView::mouseUp(const MouseEvent& ev)
{
	if (ev.eventComponent == this)
	{
		if (ev.mods.isLeftButtonDown() && !ev.source.hasMouseMovedSignificantlySincePressed())
		{
			setSelectedTrack(nullptr);
		}
	}
}

void AudioCoreView::setSelectedTrack(Track* track)
{
	if (selectedTrack == track)
		return;

	selectedTrack = track;
	mixView->setSelectedTrack(track);
	trackDetailView->setTrack(track);
	resized();
}

int AudioCoreView::getSelectedTrackIndex() const
{
	if (selectedTrack == nullptr)
		return -1;

	int i = 0;
	for (Track* track : AudioCore::getInstance()->getTracks())
	{
		if (selectedTrack == track)
			return i;

		++i;
	}

	return -1;
}

int AudioCoreView::getIndexAfterSelectedTrack() const
{
	const int selectedIndex = getSelectedTrackIndex();
	return selectedIndex < 0 ? selectedIndex : selectedIndex + 1;
}