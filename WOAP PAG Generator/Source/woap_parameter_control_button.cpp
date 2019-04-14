#include "woap_parameter_control_button.h"
#include "woap_exceptions.h"
#include "woap_entity_manager.h"
#include "woap_main_component.h"

ParameterControlButton::ParameterControlButton(PluginParameter* parameter)
	: ParameterControl(parameter)
{
	buttonLabel = new Label("buttonlabel", "S");
	buttonLabel->setEditable(false, true);
	buttonLabel->addMouseListener(this, true);
	buttonLabel->setJustificationType(Justification::centred);
	addAndMakeVisible(buttonLabel);

	parameterChanged(pluginParameter);
}

ParameterControlButton::ParameterControlButton(const ParameterControl& other)
	: ParameterControl(other)
{
	if (const ParameterControlButton* btn = dynamic_cast<const ParameterControlButton*>(&other))
	{
		buttonLabel = new Label("buttonlabel", btn->getButtonText());
	}
	else
	{
		buttonLabel = new Label("buttonlabel", "S");
	}
	
	buttonLabel->setEditable(false, true);
	buttonLabel->addMouseListener(this, true);
	buttonLabel->setJustificationType(Justification::centred);
	addAndMakeVisible(buttonLabel);

	parameterChanged(pluginParameter);
}

ParameterControlButton::~ParameterControlButton()
{
}

void ParameterControlButton::paint(Graphics& g)
{
	const Rectangle<float> bounds = getLocalBounds().toFloat();

	const float parameterValue = inverted ? 1 - pluginParameter->getValue() : pluginParameter->getValue();

	g.setColour(parameterValue > 0.5f ? colourScheme.accent : colourScheme.fill);
	g.fillRoundedRectangle(bounds, 5.0f);

	g.setColour(parameterValue > 0.5f ? colourScheme.fill : colourScheme.accent);
	g.drawRoundedRectangle(bounds, 5.0f, 2.0f);

	drawOutlineIfFocused(g);
}

void ParameterControlButton::resized()
{
	Font buttonFont = buttonLabel->getFont();
	buttonFont.setHeight(getLocalBounds().getHeight() * 0.65f);
	buttonLabel->setFont(buttonFont);
	buttonLabel->setBounds(getLocalBounds());

	ParameterControl::resized();
}

void ParameterControlButton::parameterChanged(PluginParameter*)
{
	Colour textColour;

	if (!inverted)
		textColour = pluginParameter->getValue() > 0.5f ? colourScheme.fill : colourScheme.accent;
	else
		textColour = pluginParameter->getValue() <= 0.5f ? colourScheme.fill : colourScheme.accent;

	buttonLabel->setColour(Label::ColourIds::textColourId, textColour);

	repaint();
}

void ParameterControlButton::colourSchemeChanged()
{
	parameterChanged(nullptr);
}

void ParameterControlButton::invertedChanged()
{
	parameterChanged(nullptr);
}

XmlElement* ParameterControlButton::createXml() const
{
	String style = "left: " + String(getX()) + "px; " +
		"top: " + String(getY()) + "px; " +
		"position: absolute;";
	
	XmlElement* posDiv = new XmlElement("div");
	posDiv->setAttribute("style", style);

	XmlElement* wrapper = new XmlElement("PAGParameter");
	wrapper->setAttribute("ref", pluginParameter->getRef());
	posDiv->addChildElement(wrapper);

	XmlElement* ctrl = new XmlElement("PAGButton");
	ctrl->setAttribute("width", getWidth());
	ctrl->setAttribute("height", getHeight());
	ctrl->setAttribute("text", buttonLabel->getText());
	ctrl->setAttribute("color-fill", "#" + colourScheme.fill.toDisplayString(false));
	ctrl->setAttribute("color-accent", "#" + colourScheme.accent.toDisplayString(false));
	if (inverted) ctrl->setAttribute("inverted", inverted);

	wrapper->addChildElement(ctrl);

	return posDiv;
}

ParameterControlButton* ParameterControlButton::fromXml(const XmlElement & element)
{
	const String& style = element.getStringAttribute("style");
	StringArray styleTokens;
	styleTokens.addTokens(style, ";", "");

	const XmlElement* controlElement = element.getFirstChildElement()->getFirstChildElement();

	int left = -1;
	int top = -1;
	for (String& t : styleTokens)
	{
		String token = t.trim();
		if (token.startsWith("left"))
		{
			left = token.retainCharacters("1234567890").getIntValue();
		}
		else if (token.startsWith("top"))
		{
			top = token.retainCharacters("1234567890").getIntValue();
		}
	}

	if (left == -1 || top == -1)
		throw ParsingException("Can\'t parse button wrapper element. Some style attributes missing");

	int width = controlElement->getIntAttribute("width", -1);
	int height = controlElement->getIntAttribute("height", -1);
	String ref = element.getFirstChildElement()->getStringAttribute("ref");
	bool inverted = controlElement->getBoolAttribute("inverted", false);
	String text = controlElement->getStringAttribute("text");

	if (width == -1 || height == -1 || ref.isEmpty() || text.isEmpty())
		throw ParsingException("Some arguments missing on parameter control element");

	if (!controlElement->hasAttribute("color-fill"))
		throw ParsingException("Control element has no color-fill set");

	if (!controlElement->hasAttribute("color-accent"))
		throw ParsingException("Control element has no color-accent set");

	Colour colourFill = Colour::fromString("FF" + controlElement->getStringAttribute("color-fill").getLastCharacters(6));
	Colour colourAccent = Colour::fromString("FF" + controlElement->getStringAttribute("color-accent").getLastCharacters(6));

	if (colourFill.isTransparent() || colourAccent.isTransparent())
		throw ParsingException("Wrong colours on parameter control element");

	PluginParameter* param = EntityManager::getMainComponent()->getPluginParameter(ref);
	if (param == nullptr)
		throw new ParsingException("Parameter not found, ref: " + ref);

	ParameterControlButton* output = new ParameterControlButton(param);
	output->setBounds(left, top, width, height);
	output->setColourScheme(ColourScheme(colourAccent, colourFill));
	output->setInverted(inverted);
	output->buttonLabel->setText(text, NotificationType::sendNotification);
	return output;
}
