#include "woap_window.h"

using namespace WOAP;
using namespace WOAP::GUI;

Window::Window(CloseWindowListener* callback, const String& name, Colour colour,
	const bool escapeCloses, const bool onDesktop)
	: DocumentWindow(name, colour, escapeCloses, onDesktop)
{
	owner = callback;
}

Window::~Window()
{
	clearContentComponent();
}

void Window::closeButtonPressed()
{
	requestWindowClose();
}