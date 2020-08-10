#pragma once

#include <optick.h>

namespace Plop
{
	namespace Debug
	{
		void Assert_GL();

		void TODO( const char* _pMessage = nullptr );
	}
}

#define PROFILING_INIT()		{}
#define PROFILING_SHUTDOWN()	OPTICK_SHUTDOWN()
#define PROFILING_FRAME(...)	OPTICK_FRAME(__VA_ARGS__)
#define PROFILING_THREAD(...)	OPTICK_THREAD(__VA_ARGS__)
#define PROFILING_FUNCTION()	OPTICK_EVENT()
#define PROFILING_SCOPE(...)	OPTICK_EVENT(__VA_ARGS__)