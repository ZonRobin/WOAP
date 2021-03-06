#include "woap_blank_ellipse.h"
#include "woap_exceptions.h"

BlankEllipse::BlankEllipse() 
{ 
	boundsConstrainer.setSizeLimits(15, 15, 250, 250); 
}

BlankEllipse::BlankEllipse(const BlankEllipse & other) 
	: ViewEditorElement(other) 
{ 
	boundsConstrainer.setSizeLimits(15, 15, 250, 250); 
}

void BlankEllipse::paint(Graphics& g)
{ 
	g.setColour(activeColour); 
	g.fillEllipse(getLocalBounds().toFloat()); 

	if (focused) 
	{
		g.setColour(activeColour.contrasting());
		g.drawRect(getLocalBounds());
	}
}

XmlElement* BlankEllipse::createXml() const
{
	String style = "left: " + String(getX()) + "px; " + 
		"top: " + String(getY()) + "px; " + 
		"width: " + String(getWidth()) + "px; " +
		"height: " + String(getHeight()) + "px; " +
		"position: absolute; " +
		"border-radius: 50%; " +
		"background-color: #" + activeColour.toDisplayString(false) + ";";

	XmlElement* el = new XmlElement("div");
	el->setAttribute("class", "pagEllipse");
	el->setAttribute("style", style);
	el->addTextElement("");

	return el;
}

BlankEllipse* BlankEllipse::fromXml(const XmlElement& element)
{
	const String& style = element.getStringAttribute("style");
	StringArray styleTokens;
	styleTokens.addTokens(style, ";", "");

	int left = -1;
	int top = -1;
	int width = -1;
	int height = -1;
	Colour colour;

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
		else if (token.startsWith("width"))
		{
			width = token.retainCharacters("1234567890").getIntValue();
		}
		else if (token.startsWith("height"))
		{
			height = token.retainCharacters("1234567890").getIntValue();
		}
		else if (token.startsWith("background-color"))
		{
			colour = Colour::fromString(String("FF") + token.getLastCharacters(6));
		}
	}

	if (left == -1 || top == -1 || width == -1 || height == -1)
		throw ParsingException("Can\'t parse blank ellipse element. Some style attributes missing");

	BlankEllipse* output = new BlankEllipse();
	output->setBounds(left, top, width, height);
	output->setActiveColour(colour);
	return output;
}
