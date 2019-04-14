#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_main_component.h"
#include "woap_view_editor_node.h"
#include "woap_parameter_control.h"

class ParameterControl;
class ColourPicker;
class ViewEditorElement;

class ViewEditor : public Component, public DragAndDropTarget, public MainComponent::Listener, public ViewEditorNode::Listener, public ParameterControl::ValueListener
{
public:
	class Listener
	{
	public:
		virtual void elementTouched(ViewEditorElement*) {}
		virtual void elementCreated(ViewEditorElement*) {}
		virtual void elementWillDelete(ViewEditorElement*) {}
		virtual void parameterControlTouched(ParameterControl*) {}
		virtual void parameterControlCreated(ParameterControl*) {}
		virtual void parameterControlWillDelete(ParameterControl*) {}
	};

	ViewEditor();
	virtual ~ViewEditor();

	void paint(Graphics& g) override;
	void resized() override;

	void setPluginSnapshot(Image& snapshot);

	virtual bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
	virtual void itemDropped(const SourceDetails& dragSourceDetails) override;
	void removeNode(ViewEditorNode* node);

	virtual void pluginWillBeDeleted() override;
	virtual void pluginLoaded(AudioProcessor* processor, PluginDescription* description) override;
	virtual void pluginEditorWillBeDeleted() override;
	virtual void pluginEditorCreated(AudioProcessorEditor* editor) override;
	virtual void pluginParameterTouched(PluginParameter* parameter) override;
	
	virtual void parameterControlValueChanged(ParameterControl* control, float newValue) override;
	virtual void editorNodeDeleteRequested(ViewEditorNode* node) override;
	virtual void editorNodeTouched(ViewEditorNode* node) override;

	Image& getSnapshot() { return pluginSnapshot; }
	ColourPicker* getColourPicker() { return colourPicker; }

	const OwnedArray<ParameterControl>& getParameterControls() const { return parameterControls; }

	void addListener(Listener* listenerToAdd) { listeners.add(listenerToAdd); }
	void removeListener(Listener* listenerToRemove) { listeners.remove(listenerToRemove); }

	XmlElement* createXml() const;
	HashMap<String, const Image*>* getImagesList() const;

	void loadFromXml(const XmlElement& element);

protected:
	Image pluginSnapshot;
	OwnedArray<ParameterControl> parameterControls;
	OwnedArray<ViewEditorElement> elements;
	ScopedPointer<ColourPicker> colourPicker;

	ListenerList<Listener> listeners;

	void addElementFromXml(const XmlElement& element);
	void addParameterControlFromXml(const XmlElement& element);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ViewEditor)
};