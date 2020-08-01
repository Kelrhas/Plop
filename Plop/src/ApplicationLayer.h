#pragma once

#include <Types.h>

namespace Plop
{
	class ApplicationLayer
	{
	public:

		virtual void OnRegistered() = 0;
		virtual void OnUnregistered() = 0;
		virtual void OnUpdate() = 0;

		virtual uint8_t GetPriority() const { return 0; }
	};
}
