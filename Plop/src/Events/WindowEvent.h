#pragma once

#include <Events/Event.h>

namespace Plop
{
	class WindowEvent : public Event
	{
	public:
		WindowEvent();
		~WindowEvent();
	};

	class WindowCloseEvent : public WindowEvent
	{
	public:
		MACRO_EVENT_TYPE( WindowCloseEvent );
	};

	class WindowMoveEvent : public WindowEvent
	{
	public:
		MACRO_EVENT_TYPE( WindowMoveEvent );

		int iNewPosX;
		int iNewPosY;
	};

	class WindowSizeEvent : public WindowEvent
	{
	public:
		MACRO_EVENT_TYPE( WindowSizeEvent );

		int iNewWidth;
		int iNewHeight;
	};

}
