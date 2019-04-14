#include "woap_look_and_feel.h"

WOAPLookAndFeel::WOAPLookAndFeel()
	: LookAndFeel_V4(LookAndFeel_V4::getDarkColourScheme())
{
	setColour(ResizableWindow::backgroundColourId, Colour(33, 33, 33));
	setColour(TextButton::ColourIds::buttonColourId, Colour(33, 33, 33));
	setColour(TextButton::ColourIds::buttonOnColourId, CustomColours::lightGreen);
	setColour(TextButton::ColourIds::textColourOnId, Colours::black);
	setColour(ComboBox::outlineColourId, Colour(90, 90, 90));
	setColour(Slider::ColourIds::backgroundColourId, Colour(33, 33, 33));
	setColour(Slider::ColourIds::trackColourId, Colour(50, 50, 50));
	setColour(Slider::ColourIds::thumbColourId, CustomColours::lightGreen);
	setColour(AlertWindow::backgroundColourId, Colour(33, 33, 33));

	getCurrentColourScheme().setUIColour(ColourScheme::widgetBackground, Colour(60, 60, 60));
}

WOAPLookAndFeel::~WOAPLookAndFeel()
{

}

void WOAPLookAndFeel::drawButtonBackground(Graphics& g,
	Button& button,
	const Colour& backgroundColour,
	bool,
	bool isButtonDown)
{
	const auto cornerSize = 3.0f;
	const auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

	auto baseColour = backgroundColour.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

	if (isButtonDown)
		baseColour = baseColour.contrasting(0.2f);

	g.setColour(baseColour);

	if (button.isConnectedOnLeft() || button.isConnectedOnRight())
	{
		Path path;
		path.addRoundedRectangle(bounds.getX(), bounds.getY(),
			bounds.getWidth(), bounds.getHeight(),
			cornerSize, cornerSize,
			!button.isConnectedOnLeft(),
			!button.isConnectedOnRight(),
			!button.isConnectedOnLeft(),
			!button.isConnectedOnRight());

		g.fillPath(path);

		g.setColour(button.findColour(ComboBox::outlineColourId));
		g.strokePath(path, PathStrokeType(1.0f));
	}
	else
	{
		g.fillRoundedRectangle(bounds, cornerSize);

		g.setColour(button.findColour(ComboBox::outlineColourId));
		g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
	}
}

class WOAPLookAndFeel_DocumentWindowButton : public Button
{
public:
	WOAPLookAndFeel_DocumentWindowButton(const String& name, Colour c, const Path& normal, const Path& toggled)
		: Button(name), colour(c), normalShape(normal), toggledShape(toggled)
	{
	}

	void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown) override
	{
		ignoreUnused(isMouseOverButton);
		auto background = Colours::grey;

		if (auto* rw = findParentComponentOfClass<ResizableWindow>())
			if (auto lf = dynamic_cast<LookAndFeel_V4*> (&rw->getLookAndFeel()))
				background = lf->getCurrentColourScheme().getUIColour(LookAndFeel_V4::ColourScheme::widgetBackground);

		if (isButtonDown)
		{
			g.setColour(colour);
			g.fillAll();
		}

		g.setColour(isButtonDown ? background : colour);
		auto& p = getToggleState() ? toggledShape : normalShape;

		auto reducedRect = Justification(Justification::centred)
			.appliedToRectangle(Rectangle<int>(getHeight(), getHeight()), getLocalBounds())
			.toFloat()
			.reduced(getHeight() * 0.3f);

		g.fillPath(p, p.getTransformToScaleToFit(reducedRect, true));
	}

private:
	Colour colour;
	Path normalShape, toggledShape;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WOAPLookAndFeel_DocumentWindowButton)
};

Button* WOAPLookAndFeel::createDocumentWindowButton(int buttonType)
{
	Path shape;
	const float crossThickness = 0.15f;

	if (buttonType == DocumentWindow::closeButton)
	{
		shape.addLineSegment({ 0.0f, 0.0f, 1.0f, 1.0f }, crossThickness);
		shape.addLineSegment({ 1.0f, 0.0f, 0.0f, 1.0f }, crossThickness);

		return new WOAPLookAndFeel_DocumentWindowButton("close", CustomColours::red, shape, shape);
	}

	if (buttonType == DocumentWindow::minimiseButton)
	{
		shape.addLineSegment({ 0.0f, 0.5f, 1.0f, 0.5f }, crossThickness);

		return new WOAPLookAndFeel_DocumentWindowButton("minimise", Colour(0xffaa8811), shape, shape);
	}

	if (buttonType == DocumentWindow::maximiseButton)
	{
		shape.addLineSegment({ 0.5f, 0.0f, 0.5f, 1.0f }, crossThickness);
		shape.addLineSegment({ 0.0f, 0.5f, 1.0f, 0.5f }, crossThickness);

		Path fullscreenShape;
		fullscreenShape.startNewSubPath(45.0f, 100.0f);
		fullscreenShape.lineTo(0.0f, 100.0f);
		fullscreenShape.lineTo(0.0f, 0.0f);
		fullscreenShape.lineTo(100.0f, 0.0f);
		fullscreenShape.lineTo(100.0f, 45.0f);
		fullscreenShape.addRectangle(45.0f, 45.0f, 100.0f, 100.0f);
		PathStrokeType(30.0f).createStrokedPath(fullscreenShape, fullscreenShape);

		return new WOAPLookAndFeel_DocumentWindowButton("maximise", Colour(0xff0A830A), shape, fullscreenShape);
	}

	jassertfalse;
	return nullptr;
}

void WOAPLookAndFeel::drawLabel(Graphics& g, Label& label)
{
	g.setColour(label.findColour(Label::backgroundColourId));
	g.fillRoundedRectangle(label.getLocalBounds().toFloat(), 3.0f);

	if (!label.isBeingEdited())
	{
		const float alpha = label.isEnabled() ? 1.0f : 0.5f;
		const Font font(getLabelFont(label));

		g.setColour(label.findColour(Label::textColourId).withMultipliedAlpha(alpha));
		g.setFont(font);

		Rectangle<int> textArea(label.getBorderSize().subtractedFrom(label.getLocalBounds()));

		g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
			jmax(1, (int)(textArea.getHeight() / font.getHeight())),
			label.getMinimumHorizontalScale());

		g.setColour(label.findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
	}
	else if (label.isEnabled())
	{
		g.setColour(label.findColour(Label::outlineColourId));
	}

	g.drawRect(label.getLocalBounds());
}

const Colour CustomColours::red(0xFFE12525);
const Colour CustomColours::orange(0xFFF5A22E);
const Colour CustomColours::yellow(0xFFF0E223);
const Colour CustomColours::lightGreen(0xFF7AD975);
const Colour CustomColours::green(0xFF1FBA46);
const Colour CustomColours::lightBlue(0xFF00C1E8);
const Colour CustomColours::blue(0xFF4967F2);
const Colour CustomColours::violet(0xFF7239B8);
const Colour CustomColours::grey(0xFF808080);
const Colour CustomColours::black(0xFF1E1E1E);
const Colour CustomColours::white(0xFFDCDCDC);

const Colour CustomColours::audioTrackColour(CustomColours::green);
const Colour CustomColours::mixBusTrackColour(CustomColours::blue);
const Colour CustomColours::fxTrackColour(CustomColours::lightBlue);
const Colour CustomColours::outputBusTrackColour(CustomColours::yellow);
const Colour CustomColours::lightTextColour(0xFFFFFFFF);
const Colour CustomColours::darkTextColour(0XFF1A1A1A);

const Array<Colour> CustomColours::trackColourPalette({
	CustomColours::red,
	CustomColours::orange,
	CustomColours::yellow,
	CustomColours::lightGreen,
	CustomColours::green,
	CustomColours::lightBlue,
	CustomColours::blue,
	CustomColours::violet,
	CustomColours::grey,
	CustomColours::black,
	CustomColours::white,
});