#include "woap_view_editor.h"
#include "woap_main.h"
#include "woap_parameter_control_rotary_slider.h"
#include "woap_parameter_control_linear_slider.h"
#include "woap_parameter_control_button.h"
#include "woap_entity_manager.h"
#include "woap_colour_picker.h"
#include "woap_blank_ellipse.h"
#include "woap_blank_rectangle.h"
#include "woap_exceptions.h"

ViewEditor::ViewEditor()
{
	EntityManager::getMainComponent()->addListener(this);

	colourPicker = new ColourPicker();
	colourPicker->setAlwaysOnTop(true);
	addChildComponent(colourPicker);
}

ViewEditor::~ViewEditor()
{
	EntityManager::getMainComponent()->removeListener(this);
}

void ViewEditor::paint(Graphics& g)
{
	g.drawImageAt(pluginSnapshot, 0, 0);
}

void ViewEditor::resized()
{
	colourPicker->setBounds(getLocalBounds());
}

void ViewEditor::setPluginSnapshot(Image& snapshot)
{
	pluginSnapshot = snapshot;
	repaint();
}

bool ViewEditor::isInterestedInDragSource(const SourceDetails& dragSourceDetails)
{
	return !pluginSnapshot.isNull() && nullptr != dynamic_cast<ViewEditorNode*>(dragSourceDetails.sourceComponent.get());
}

void ViewEditor::itemDropped(const SourceDetails& dragSourceDetails)
{
	Component* sourceComponent = dragSourceDetails.sourceComponent.get();
	
	ViewEditorNode* newNode = nullptr;
	int zIndex = -1;
	
	if (ParameterControl* control = dynamic_cast<ParameterControl*>(sourceComponent))
	{
		ParameterControl* newControl;

		if (dynamic_cast<ParameterControlLinearSlider*>(control))
		{
			newNode = newControl = new ParameterControlLinearSlider(*control);
		}
		else if (dynamic_cast<ParameterControlRotarySlider*>(control))
		{
			newNode = newControl = new ParameterControlRotarySlider(*control);
		}
		else if (dynamic_cast<ParameterControlButton*>(control))
		{
			newNode = newControl = new ParameterControlButton(*control);
		}
		else
		{
			jassertfalse;
			return;
		}
		
		newControl->addValueListener(this);
		parameterControls.add(newControl);
		listeners.call(&ViewEditor::Listener::parameterControlCreated, newControl);
	}
	else if (ViewEditorElement* element = dynamic_cast<ViewEditorElement*>(sourceComponent))
	{
		ViewEditorElement* newElement; 

		if (BlankEllipse* ellipse = dynamic_cast<BlankEllipse*>(element))
		{
			newNode = newElement = new BlankEllipse(*ellipse);
		}
		else if (BlankRectangle* rectangle = dynamic_cast<BlankRectangle*>(element))
		{
			newNode = newElement = new BlankRectangle(*rectangle);
		}
		else
		{
			jassertfalse;
			return;
		}

		zIndex = elements.size();
		elements.add(newElement);
		listeners.call(&ViewEditor::Listener::elementCreated, newElement);
	}

	if (newNode == nullptr)
	{
		jassertfalse;
		return;
	}

	addAndMakeVisible(newNode, zIndex);
	newNode->setDragEnabled(true);
	newNode->setResizeEnabled(true);
	newNode->setDeleteAllowed(true);
	newNode->addListener(this);
	newNode->setBounds(
		dragSourceDetails.localPosition.getX() - (int)sourceComponent->getWidth() / 2,
		dragSourceDetails.localPosition.getY() - (int)sourceComponent->getHeight() / 2,
		sourceComponent->getWidth(),
		sourceComponent->getHeight()
	);
	newNode->checkBounds();
	newNode->simulateTouch();
}

void ViewEditor::removeNode(ViewEditorNode* nodeToDelete)
{	
	if (ParameterControl* control = dynamic_cast<ParameterControl*>(nodeToDelete))
	{
		ScopedPointer<ViewEditorNode> node(control);
		parameterControls.removeObject(control, false);
		removeChildComponent(control);
		listeners.call(&ViewEditor::Listener::parameterControlWillDelete, control);
	}
	else if (ViewEditorElement* element = dynamic_cast<ViewEditorElement*>(nodeToDelete))
	{
		ScopedPointer<ViewEditorNode> node(element);
		elements.removeObject(element, false);
		removeChildComponent(element);
		listeners.call(&ViewEditor::Listener::elementWillDelete, element);
	}
	else
	{
		jassertfalse;
		return;
	}
}

void ViewEditor::pluginWillBeDeleted()
{
	parameterControls.clearQuick(true);
}

void ViewEditor::pluginLoaded(AudioProcessor*, PluginDescription*)
{
}

void ViewEditor::pluginEditorWillBeDeleted()
{
}

void ViewEditor::pluginEditorCreated(AudioProcessorEditor*)
{
}

void ViewEditor::pluginParameterTouched(PluginParameter* parameter)
{
	for (ParameterControl* control : parameterControls)
	{
		if (control->getPluginParameter() == parameter)
		{
			control->setFocused(true);
		}
		else
		{
			control->setFocused(false);
		}
	}
}

void ViewEditor::parameterControlValueChanged(ParameterControl* control, float newValue)
{
	EntityManager::getMainComponent()->setParameterValue(control->getPluginParameter()->getIndex(), newValue);
}

void ViewEditor::editorNodeDeleteRequested(ViewEditorNode* node)
{
	MessageManager::callAsync([this, node]() {
		removeNode(node);
	});
}

void ViewEditor::editorNodeTouched(ViewEditorNode* node)
{
	if (ViewEditorElement* el = dynamic_cast<ViewEditorElement*>(node))
	{
		for (ViewEditorElement* element : elements)
		{
			element->setFocused(false);
		}

		node->setFocused(true);
		listeners.call(&ViewEditor::Listener::elementTouched, el);
	}
	else if (ParameterControl* control = dynamic_cast<ParameterControl*>(node))
	{
		EntityManager::getMainComponent()->parameterTouched(control->getPluginParameter()->getIndex());
		listeners.call(&ViewEditor::Listener::parameterControlTouched, control);
	}
}

XmlElement* ViewEditor::createXml() const
{
	XmlElement* view = new XmlElement("view");
	view->setAttribute("default", true);
	view->setAttribute("name", "PAG view 1");
	view->setAttribute("lang", "pag-jsx");
	
	XmlElement* rootDiv = new XmlElement("div");
	rootDiv->setAttribute("class", "pagView");
	rootDiv->setAttribute("style",
		"width: " + String(pluginSnapshot.getWidth()) + "px; " +
		"height: " + String(pluginSnapshot.getHeight()) + "px; " +
		"position: relative;"
	);
	view->addChildElement(rootDiv);

	XmlElement* snapshotDiv = new XmlElement("PAGImage");
	snapshotDiv->setAttribute("ref", "background");
	rootDiv->addChildElement(snapshotDiv);

	for (ViewEditorElement* el : elements)
	{
		rootDiv->addChildElement(el->createXml());
	}

	for (ParameterControl* ctrl : parameterControls)
	{
		rootDiv->addChildElement(ctrl->createXml());
	}

	return view;
}

HashMap<String, const Image*>* ViewEditor::getImagesList() const
{
	if (pluginSnapshot.isNull()) {
		return new HashMap<String, const Image*>();
	} else {
		HashMap<String, const Image*>* a = new HashMap<String, const Image*>();
		a->set("background", &pluginSnapshot);
		return a;
	}
}

void ViewEditor::loadFromXml(const XmlElement& viewElement)
{
	OwnedArray<ParameterControl> ctrls;
	for (ParameterControl* control : parameterControls)
	{
		ctrls.add(control);
	}
	parameterControls.clearQuick(false);
	for (ParameterControl* control : parameterControls)
	{
		listeners.call(&ViewEditor::Listener::parameterControlWillDelete, control);
	}
	
	OwnedArray<ViewEditorElement> elems;
	for (ViewEditorElement* element : elements)
	{
		elems.add(element);
	}
	elements.clearQuick(false);
	for (ViewEditorElement* element : elements)
	{
		listeners.call(&ViewEditor::Listener::elementWillDelete, element);
	}	
	
	if (viewElement.getNumChildElements() == 0 || viewElement.getFirstChildElement()->getStringAttribute("class") != "pagView")
		throw ParsingException("There\'s pagView div missing");

	const XmlElement* viewChilds = viewElement.getFirstChildElement();
	forEachXmlChildElement(*viewChilds, viewChild)
	{
		if (viewChild->isTextElement()) continue;

		if (!viewChild->hasTagName("div"))
		{
			if (viewChild->hasTagName("PAGImage")) continue;

			throw ParsingException("Unknown element " + viewChild->getTagName());
		}

		if (viewChild->getNumChildElements() == 0)
		{
			addElementFromXml(*viewChild);
		}
		else if (viewChild->getNumChildElements() == 1)
		{
			addParameterControlFromXml(*viewChild);
		}
		else
		{
			throw ParsingException("View child can\'t have more than one child element");
		}
	}
}

void ViewEditor::addElementFromXml(const XmlElement& element)
{
	const String& className = element.getStringAttribute("class");
	
	ViewEditorElement* editorElement;
	if (className == "pagRectangle")
	{
		editorElement = BlankRectangle::fromXml(element);
	}
	else if (className == "pagEllipse")
	{
		editorElement = BlankEllipse::fromXml(element);
	}
	else
	{
		throw ParsingException("Unknown div element");
	}
	
	elements.add(editorElement);
	const int zIndex = elements.size();
	addAndMakeVisible(editorElement, zIndex);
	editorElement->setDragEnabled(true);
	editorElement->setResizeEnabled(true);
	editorElement->setDeleteAllowed(true);
	editorElement->addListener(this);
	editorElement->checkBounds();
}

void ViewEditor::addParameterControlFromXml(const XmlElement& element)
{
	if (element.getFirstChildElement()->getTagName() != "PAGParameter")
		throw ParsingException("Unknown parameter control child element");

	if (element.getFirstChildElement()->getNumChildElements() == 0)
		throw ParsingException("PAGParameter must have a control element");

	const String& controlTagName = element.getFirstChildElement()->getFirstChildElement()->getTagName();

	ParameterControl* parameterControl;
	if (controlTagName == "PAGLinearSlider")
	{
		parameterControl = ParameterControlLinearSlider::fromXml(element);
	}
	else if (controlTagName == "PAGRotarySlider")
	{
		parameterControl = ParameterControlRotarySlider::fromXml(element);
	}
	else if (controlTagName == "PAGButton")
	{
		parameterControl = ParameterControlButton::fromXml(element);
	}
	else
	{
		throw ParsingException("Unknown parameter control element: " + controlTagName);
	}

	parameterControls.add(parameterControl);
	parameterControl->addValueListener(this);
	addAndMakeVisible(parameterControl);
	parameterControl->setDragEnabled(true);
	parameterControl->setResizeEnabled(true);
	parameterControl->setDeleteAllowed(true);
	parameterControl->addListener(this);
	parameterControl->checkBounds();
}
