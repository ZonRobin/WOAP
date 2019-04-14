#include "woap_drag_handle.h"

using namespace WOAP;
using namespace WOAP::GUI;

DragHandle::DragHandle()
{
	setViewportIgnoreDragFlag(true);
	setMouseCursor(MouseCursor::DraggingHandCursor);
}

DragHandle::~DragHandle()
{
}

void DragHandle::paint(Graphics& g)
{
	const float glyphSize = jmin(getWidth(), getHeight()) * 0.66f;
	FontAwesome::drawCenterd(g, FontAwesome_Bars, glyphSize, Colours::black, getLocalBounds());
}
