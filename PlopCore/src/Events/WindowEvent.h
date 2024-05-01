#pragma once

#include <Events/Event.h>
#include <Window.h>

namespace Plop
{
	class WindowEvent : public Event
	{
	public:
		WindowEvent( const Window& _window );
		~WindowEvent();

		const Window& window;
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		WindowCloseEvent( const Window& _window ) : WindowEvent( _window ) {}
		MACRO_EVENT_TYPE( WindowCloseEvent );
	};

	class WindowMoveEvent : public WindowEvent
	{
	public:
		WindowMoveEvent( const Window& _window ) : WindowEvent( _window ) {}
		MACRO_EVENT_TYPE( WindowMoveEvent );

		int iNewPosX;
		int iNewPosY;
	};

	class WindowSizeEvent : public WindowEvent
	{
	public:
		WindowSizeEvent( const Window& _window ) : WindowEvent( _window ) {}
		MACRO_EVENT_TYPE( WindowSizeEvent );

		int iNewWidth;
		int iNewHeight;
	};

}
