#include "woap_view_editor_node.h"
#include "woap_entity_manager.h"
#include "woap_main_component.h"
#include "woap_view_editor.h"

void ViewEditorNode::mouseDown(const MouseEvent& event)
{
	if (event.mods.isPopupMenu())
	{
		if (!deleteAllowed) return;

		PopupMenu menu;
		menu.addItem(1, String("Delete"), true, false);
		const int actionId = menu.show();

		if (actionId == 1)
		{
			listeners.call(&Listener::editorNodeDeleteRequested, this);
		}
	}
	else if (event.mods.isLeftButtonDown())
	{
		if (dragAndDropEnabled)
		{
			EntityManager::getMainComponent()->startDragging(var(), this, createComponentSnapshot(getLocalBounds()));
		}
		else if (dragEnabled)
		{
			dragger.startDraggingComponent(this, event);
		}
	}

	listeners.call(&Listener::editorNodeTouched, this);
}

void ViewEditorNode::mouseDrag(const MouseEvent& event)
{
	if (event.mods.isLeftButtonDown() && dragEnabled)
	{
		dragger.dragComponent(this, event, &boundsConstrainer);
	}
}

void ViewEditorNode::setResizeEnabled(bool enabled)
{
	resizeEnabled = enabled;

	if (focused)
	{
		resizer->setVisible(true);
	}
}

void ViewEditorNode::setFocused(bool val)
{
	focused = val;

	if (resizeEnabled && val) 
	{
		resizer->setVisible(true);
	}
	else
	{
		resizer->setVisible(false);
	}
	
	repaint();
}

void ViewEditorNode::checkBounds()
{
	boundsConstrainer.checkComponentBounds(this);
}

void ViewEditorNode::simulateTouch()
{
	listeners.call(&Listener::editorNodeTouched, this);
}