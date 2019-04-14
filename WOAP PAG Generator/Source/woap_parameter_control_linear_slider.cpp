#include "woap_parameter_control_linear_slider.h"
#include "woap_exceptions.h"
#include "woap_main_component.h"
#include "woap_entity_manager.h"

ParameterControlLinearSlider::ParameterControlLinearSlider(Orientation o, PluginParameter* parameter)
	: ParameterControl(parameter), orientation(o)
{
	if (isHorizontal())
		boundsConstrainer.setSizeLimits(30, 10, 250, 30);
	else 
		boundsConstrainer.setSizeLimits(10, 30, 30, 250);
}

ParameterControlLinearSlider::ParameterControlLinearSlider(const ParameterControl& other)
	: ParameterControl(other)
{
	if (const ParameterControlLinearSlider* sl = dynamic_cast<const ParameterControlLinearSlider*>(&other))
	{
		orientation = sl->orientation;
		valueBarShown = sl->valueBarShown;
	}
	else
	{
		jassertfalse;
		return;
	}

	if (isHorizontal())
		boundsConstrainer.setSizeLimits(30, 10, 250, 30);
	else
		boundsConstrainer.setSizeLimits(10, 30, 30, 250);
}

ParameterControlLinearSlider::~ParameterControlLinearSlider()
{

}

void ParameterControlLinearSlider::paint(Graphics& g)
{
	const Rectangle<int> bounds = getLocalBounds();
	const int width = bounds.getWidth();
	const int height = bounds.getHeight();

	const float parameterValue = inverted ? 1 - pluginParameter->getValue() : pluginParameter->getValue();
	const float trackSize = isHorizontal() ? height * 0.5f : width * 0.5f;
	const float thumbRadius = isHorizontal() ? (float)height * 0.5f : (float)width * 0.5f;

	Rectangle<float> backgroundRect;
	Rectangle<float> valueRect;
	Point<float> valuePoint;

	if (isHorizontal())
	{
		const float valuePos = (width - thumbRadius * 2) * parameterValue + thumbRadius;
		backgroundRect = Rectangle<float>(0, (height - trackSize) * 0.5f, (float)width, trackSize);
		valueRect = backgroundRect.withWidth(valuePos);
		valuePoint = Point<float>(valuePos, height * 0.5f);
	}
	else
	{
		const float valuePos = (height - thumbRadius * 2) * (1 - parameterValue) + thumbRadius;
		backgroundRect = Rectangle<float>((width - trackSize) * 0.5f, 0, trackSize, (float)height);
		valueRect = backgroundRect.withTrimmedTop(valuePos);
		valuePoint = Point<float>(width * 0.5f, valuePos);
	}

	g.setColour(colourScheme.fill);
	g.fillRoundedRectangle(backgroundRect, 5.0f);
	
	if (valueBarShown)
	{
		g.setColour(colourScheme.accent);
		g.fillRoundedRectangle(valueRect, 5.0f);
	}

	g.setColour(colourScheme.accent);
	g.fillEllipse(Rectangle<float>(thumbRadius * 2, thumbRadius * 2).withCentre(valuePoint));

	drawOutlineIfFocused(g);
}

XmlElement* ParameterControlLinearSlider::createXml() const
{
	String style = "left: " + String(getX()) + "px; " +
		"top: " + String(getY()) + "px; " +
		"position: absolute;";

	XmlElement* posDiv = new XmlElement("div");
	posDiv->setAttribute("style", style);

	XmlElement* wrapper = new XmlElement("PAGParameter");
	wrapper->setAttribute("ref", pluginParameter->getRef());
	posDiv->addChildElement(wrapper);

	XmlElement* ctrl = new XmlElement("PAGLinearSlider");
	ctrl->setAttribute("width", getWidth());
	ctrl->setAttribute("height", getHeight());
	ctrl->setAttribute("color-fill", "#" + colourScheme.fill.toDisplayString(false));
	ctrl->setAttribute("color-accent", "#" + colourScheme.accent.toDisplayString(false));
	if (!valueBarShown) ctrl->setAttribute("value-bar", valueBarShown);
	if (inverted) ctrl->setAttribute("inverted", inverted);

	wrapper->addChildElement(ctrl);

	return posDiv;
}

ParameterControlLinearSlider* ParameterControlLinearSlider::fromXml(const XmlElement& element)
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
	bool valueBar = controlElement->getBoolAttribute("value-bar", true);

	if (width == -1 || height == -1 || ref.isEmpty())
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
		throw ParsingException("Parameter not found, ref: " + ref);

	Orientation orient = width > height ? Orientation::Horizontal : Orientation::Vertical;

	ParameterControlLinearSlider* output = new ParameterControlLinearSlider(orient, param);
	output->setBounds(left, top, width, height);
	output->setColourScheme(ColourScheme(colourAccent, colourFill));
	output->setInverted(inverted);
	output->showValueBar(valueBar);
	return output;
}
