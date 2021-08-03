#pragma once

#include <Types.h>
#include <TimeStep.h>

namespace Plop
{
	class ApplicationLayer
	{
	public:

		virtual void OnRegistered() = 0;
		virtual void OnUnregistered() = 0;
		virtual void OnUpdate(TimeStep& _timeStep) = 0;
		virtual void OnImGuiRender( TimeStep& _timeStep ) {};

		virtual uint8_t GetPriority() const { return 0; }
	};
}
