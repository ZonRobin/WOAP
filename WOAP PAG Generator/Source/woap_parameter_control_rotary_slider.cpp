#include "woap_parameter_control_rotary_slider.h"
#include "woap_exceptions.h"
#include "woap_main_component.h"
#include "woap_entity_manager.h"

ParameterControlRotarySlider::ParameterControlRotarySlider(PluginParameter* parameter)
	: ParameterControl(parameter)
{
	boundsConstrainer.setSizeLimits(15, 15, 250, 250);
	boundsConstrainer.setFixedAspectRatio(1);
}

ParameterControlRotarySlider::ParameterControlRotarySlider(const ParameterControl& other)
	: ParameterControl(other)
{
	boundsConstrainer.setSizeLimits(15, 15, 250, 250);
	boundsConstrainer.setFixedAspectRatio(1);
}

ParameterControlRotarySlider::~ParameterControlRotarySlider()
{

}

void ParameterControlRotarySlider::paint(Graphics& g)
{
	const Rectangle<float> bounds = getLocalBounds().toFloat();
	const float rotaryStartAngle = MathConstants<float>::pi;
	const float rotaryEndAngle = 3 * MathConstants<float>::pi;

	const float parameterValue = inverted ? 1 - pluginParameter->getValue() : pluginParameter->getValue();
	const float radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
	const float toAngle = rotaryStartAngle + parameterValue * (rotaryEndAngle - rotaryStartAngle);
	const float lineW = bounds.getWidth() * 0.05f;
	const float arcRadius = radius - lineW * 0.5f;

	const Point<float> valuePoint(bounds.getCentreX() + radius * std::cos(toAngle - float_Pi * 0.5f),
		bounds.getCentreY() + radius * std::sin(toAngle - float_Pi * 0.5f));

	g.setColour(colourScheme.fill);
	g.fillEllipse(bounds.toFloat());

	Path valueBackground;
	valueBackground.addPieSegment(bounds, rotaryStartAngle, toAngle, 0);

	g.setColour(colourScheme.accent.withAlpha(0.4f));
	g.fillPath(valueBackground);

	g.setColour(colourScheme.accent);
	g.fillEllipse(bounds.withSizeKeepingCentre(bounds.getWidth() * 0.16f, bounds.getWidth() * 0.16f));

	Path valueArc;
	valueArc.addCentredArc(bounds.getCentreX(),
		bounds.getCentreY(),
		arcRadius,
		arcRadius,
		0.0f,
		rotaryStartAngle,
		toAngle,
		true);

	g.setColour(colourScheme.accent);
	g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::mitered, PathStrokeType::butt));
	g.drawLine(valuePoint.getX(), valuePoint.getY(), bounds.getWidth() / 2, bounds.getHeight() / 2, lineW);

	drawOutlineIfFocused(g);
}

XmlElement* ParameterControlRotarySlider::createXml() const
{
	String style = "left: " + String(getX()) + "px; " +
		"top: " + String(getY()) + "px; " +
		"position: absolute;";

	XmlElement* posDiv = new XmlElement("div");
	posDiv->setAttribute("style", style);

	XmlElement* wrapper = new XmlElement("PAGParameter");
	wrapper->setAttribute("ref", pluginParameter->getRef());
	posDiv->addChildElement(wrapper);

	XmlElement* ctrl = new XmlElement("PAGRotarySlider");
	ctrl->setAttribute("size", getWidth());
	ctrl->setAttribute("color-fill", "#" + colourScheme.fill.toDisplayString(false));
	ctrl->setAttribute("color-accent", "#" + colourScheme.accent.toDisplayString(false));
	if (inverted) ctrl->setAttribute("inverted", inverted);

	wrapper->addChildElement(ctrl);

	return posDiv;
}

ParameterControlRotarySlider * ParameterControlRotarySlider::fromXml(const XmlElement & element)
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

	int size = controlElement->getIntAttribute("size", -1);
	String ref = element.getFirstChildElement()->getStringAttribute("ref");
	bool inverted = controlElement->getBoolAttribute("inverted", false);

	if (size == -1 || ref.isEmpty())
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

	ParameterControlRotarySlider* output = new ParameterControlRotarySlider(param);
	output->setBounds(left, top, size, size);
	output->setColourScheme(ColourScheme(colourAccent, colourFill));
	output->setInverted(inverted);
	return output;
}
