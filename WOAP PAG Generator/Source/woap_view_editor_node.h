#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ViewEditorNode : public Component
{
public:
	class Listener
	{
	public:
		virtual void editorNodeDeleteRequested(ViewEditorNode* node) = 0;
		virtual void editorNodeTouched(ViewEditorNode* node) = 0;
	};

	ViewEditorNode() 
	{
		resizer = new ResizableBorderComponent(this, &boundsConstrainer);
		resizer->setAlwaysOnTop(true);
		addChildComponent(resizer);

		boundsConstrainer.setMinimumOnscreenAmounts(0xffffff, 0xffffff, 0xffffff, 0xffffff);
	}
	ViewEditorNode(const ViewEditorNode&) {}
	~ViewEditorNode() {}

	virtual void resized() override
	{
		resizer->setBounds(getLocalBounds());
	}

	virtual void mouseDown(const MouseEvent& e) override;
	virtual void mouseDrag(const MouseEvent& e) override;

	void setDragAndDropEnabled(bool enabled) { dragAndDropEnabled = enabled; }
	void setDragEnabled(bool enabled) { dragEnabled = enabled; }
	void setDeleteAllowed(bool allowed) { deleteAllowed = allowed; }
	void setResizeEnabled(bool enabled);
	void setFocused(bool val);

	void checkBounds();
	void simulateTouch();

	void addListener(Listener* listenerToAdd) { listeners.add(listenerToAdd); }
	void removeListener(Listener* listenerToRemove) { listeners.remove(listenerToRemove); }

	virtual XmlElement* createXml() const = 0;

protected:
	bool dragAndDropEnabled = false;
	bool dragEnabled = false;
	bool resizeEnabled = false;
	bool focused = false;
	bool deleteAllowed = false;

	ComponentBoundsConstrainer boundsConstrainer;
	ScopedPointer<ResizableBorderComponent> resizer;
	ComponentDragger dragger;

	ListenerList<Listener> listeners;
	
private:
	JUCE_LEAK_DETECTOR(ViewEditorNode)
};