#include "woap_pan_view.h"
#include "woap_parameter_float.h"
#include "woap_panner.h"

using namespace WOAP;
using namespace WOAP::GUI;

PanView::PanView(ParameterFloat* p) 
	: parameter(nullptr)
{
	setMouseCursor(MouseCursor::PointingHandCursor);
	setRepaintsOnMouseActivity(false);
	setParameter(p);
}

PanView::~PanView()
{
	if (parameter != nullptr)
		parameter->removeListener(this);
}

void PanView::setParameter(ParameterFloat* param)
{
	if (parameter != nullptr)
		parameter->removeListener(this);

	parameter = param;

	if (param != nullptr) 
	{
		param->addListener(this);
		setRange(param->getMinValue(), param->getMaxValue(), 0.01f);
		setValue(param->get(), NotificationType::dontSendNotification);
	}
}

void PanView::valueChanged()
{
	if (parameter != nullptr)
		*parameter = (float)getValue();
}

void PanView::startedDragging()
{
	dragging = true;
}

void PanView::stoppedDragging()
{
	dragging = false;
	setValue(parameter->get(), NotificationType::dontSendNotification);
}

void PanView::parameterChanged(Parameter*)
{
	if (!dragging)
		setValue((double)parameter->get(), NotificationType::dontSendNotification);
}