#include "woap_plugin_editor_view.h"
#include "woap_button_view.h"
#include "woap_track.h"
#include "woap_name_label.h"

using namespace WOAP;
using namespace WOAP::GUI;

PluginEditorView::PluginEditorView(CloseWindowListener* own, AudioProcessorPlayerDouble* pl)
	: plugin(pl), Window(own, pl->getTrack()->getName() + " - " + pl->getName(), Colours::black, false)
{
	setDraggable(true);
	setTitleBarHeight(0);

	editorWrapper = new EditorWrapper(plugin);
	editorWrapper->addMouseListener(this, true);
	setContentOwned(editorWrapper, true);

	plugin->addListener(this);
}

PluginEditorView::~PluginEditorView()
{
	plugin->removeListener(this);
}

void PluginEditorView::resized()
{
	Window::resized();

	const int width = getWidth();
	const int height = getHeight();
	getConstrainer()->setMinimumOnscreenAmounts(height, width, height, width);

	const Point<int> lastEditorPosition = plugin->getLastPluginEditorPosition();
	if (lastEditorPosition.x >= 0 && lastEditorPosition.y >= 0)
		setTopLeftPosition(lastEditorPosition);
}

void PluginEditorView::mouseDown(const MouseEvent& ev)
{
	if (ev.mods.isLeftButtonDown() && editorWrapper->canDragEditor(ev))
		Window::mouseDown(ev);
}

void PluginEditorView::mouseUp(const MouseEvent& ev)
{
	Window::mouseUp(ev);
	plugin->setLastPluginEditorPosition(getPosition());
}

void PluginEditorView::aboutToDelete(AudioProcessorPlayerDouble*)
{
	requestWindowClose();
}




PluginEditorView::EditorWrapper::EditorWrapper(AudioProcessorPlayerDouble* pl)
	: plugin(pl)
{
	menuLineHeight = 40;
	menuHeight = menuLineHeight;
	minWidth = 200;
	menuLineBreakHeight = 350;

	nameLabel = new NameLabel();
	nameLabel->setFontSize(18.0f);
	nameLabel->setEditable(false, true);
	nameLabel->addMouseListener(this, false);
	nameLabel->addListener(this);
	addAndMakeVisible(nameLabel);

	nameChanged(pl);

	bypassButton = new BypassButtonView(plugin, "Bypass");
	addAndMakeVisible(bypassButton);

	activeStateParameter = new ActiveButtonView(plugin, "Active");
	addAndMakeVisible(activeStateParameter);

	pluginEditor = plugin->createEditor();
	addAndMakeVisible(pluginEditor);

	plugin->getTrack()->addListener(this);
	plugin->addListener(this);

	LookAndFeel& lf = getLookAndFeel();
	closeButton = lf.createDocumentWindowButton(DocumentWindow::closeButton);
	closeButton->addListener(this);
	addAndMakeVisible(closeButton);

	childBoundsChanged(pluginEditor);
}

PluginEditorView::EditorWrapper::~EditorWrapper()
{
	plugin->getTrack()->removeListener(this);
	plugin->removeListener(this);
}

void PluginEditorView::EditorWrapper::resized()
{
	const int width = getWidth();
	if (width < menuLineBreakHeight)
	{
		const int marginLeft = (int)((width - 170) * 0.5f);
		activeStateParameter->setBounds(marginLeft, 5 + menuLineHeight, 80, menuLineHeight - 10);
		bypassButton->setBounds(marginLeft + 90, 5 + menuLineHeight, 80, menuLineHeight - 10);
		nameLabel->setBounds(10, 5, width - 50, menuLineHeight - 10);
	}
	else
	{
		activeStateParameter->setBounds(10, 5, 80, menuLineHeight - 10);
		bypassButton->setBounds(100, 5, 80, menuLineHeight - 10);
		nameLabel->setBounds(190, 5, width - 230, menuLineHeight - 10);
	}

	closeButton->setBounds(width - 35, 5, 30, 30);

	pluginEditor->setCentrePosition(width / 2, (getHeight() - menuHeight) / 2 + menuHeight);
}

void PluginEditorView::EditorWrapper::paint(Graphics& g)
{
	g.fillAll(findColour(ResizableWindow::ColourIds::backgroundColourId));
}

void PluginEditorView::EditorWrapper::childBoundsChanged(Component* child)
{
	if (child != pluginEditor)
		return;

	if (child->getWidth() < menuLineBreakHeight)
		menuHeight = 2 * menuLineHeight;
	else
		menuHeight = menuLineHeight;

	setSize(jmax(child->getWidth(), minWidth), child->getHeight() + menuHeight);
}

bool PluginEditorView::EditorWrapper::canDragEditor(const MouseEvent& ev)
{
	return !nameLabel->isBeingEdited() && (ev.eventComponent == nameLabel || ev.eventComponent == this);
}

void PluginEditorView::EditorWrapper::nameChanged(AudioProcessorPlayerDouble*)
{
	nameLabel->setText(plugin->getTrack()->getName() + " - " + plugin->getName(), NotificationType::dontSendNotification);
}

void PluginEditorView::EditorWrapper::nameChanged(MixerNode*)
{
	nameLabel->setText(plugin->getTrack()->getName() + " - " + plugin->getName(), NotificationType::dontSendNotification);
}

void PluginEditorView::EditorWrapper::editorShown(Label*, TextEditor& editor)
{
	editor.setText(plugin->getName());
	editor.setCaretPosition(0);
	editor.moveCaretToEnd(true);
}

void PluginEditorView::EditorWrapper::editorHidden(Label*, TextEditor& editor)
{
	const String newName = editor.getText();

	if (newName != plugin->getName())
		plugin->setName(newName);
	else
		nameChanged(plugin); /* Refresh label (in case it wasnt changed)*/
}

void PluginEditorView::EditorWrapper::buttonClicked(Button* btn)
{
	if (btn == closeButton)
	{
		static_cast<PluginEditorView*>(getParentComponent())->requestWindowClose();
	}
}
