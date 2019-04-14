#include "woap_window.h"

Window::Window(CloseWindowListener* callback, const String& name, Colour colour,
	const bool escapeCloses, const bool onDesktop)
	: DocumentWindow(name, colour, escapeCloses, onDesktop)
{
	this->owner = callback;
}

Window::~Window()
{
	clearContentComponent();
}

void Window::closeButtonPressed()
{
	this->owner->closeWindowTriggered(this);
}