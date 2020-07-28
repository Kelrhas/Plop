#pragma once

#include <Events/Event.h>

namespace Plop
{
	class IEventListener
	{
	public:
		virtual bool OnEvent(Event& _event) = 0;
		
	private:
	};
}