#include "Plop_pch.h"
#include "WindowEvent.h"


namespace Plop
{
	WindowEvent::WindowEvent( const Window& _window ) : Event(), window(_window)
	{}


	WindowEvent::~WindowEvent()
	{}
}