#pragma once

#include <Events/Event.h>

namespace Plop
{
	class IEventListener
	{
	public:
		virtual bool OnEvent(Event *_pEvent) = 0;

	private:
	};
} // namespace Plop