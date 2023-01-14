#pragma once

#include <Config.h>
#include <optick.h>

#include <Debug/Log.h>


#ifdef ENABLE_MEMORY_TRACKING
void* operator new(size_t _Size);
void* operator new(size_t _Size, const char* _pFile, const size_t _Line);
void* operator new[]( size_t _Size );
void* operator new[](size_t _Size, const char* _pFile, const size_t _Line);
void operator delete(void* _Ptr);
void operator delete(void* _Ptr, const char* _pFile, const size_t _Line);
void operator delete[](void* _Ptr);
void operator delete[](void* _Ptr, const char* _pFile, const size_t _Line);

#define NEW new(__FILE__, __LINE__)
#else
#define NEW new
#endif // ENABLE_MEMORY_TRACKING

namespace Plop
{
	namespace Debug
	{
		extern bool bBreakOnAssert;

		void NewFrame();
		void EndFrame();

		void Assert_GL();
		void Assert_AL();

		void TODO( const char* _pMessage = nullptr );
#define TODO_STATIC( message ) static_assert(false, message);

		void ShowAllocationsWindow( bool* _bOpened = nullptr );
	}
}




#define COMPILE_ASSERT(b, m) static_assert(b, m);
#define ASSERT_COMPILE_TIME(b, m) COMPILE_ASSERT(b, m);

#ifndef _MASTER
	#define BREAK()						__debugbreak()
	#define ASSERT(action)				{bool __b = (bool)(action); if(!__b){ static bool __bIgnore = false; if(::Plop::Debug::bBreakOnAssert &&!__bIgnore) BREAK(); }}
	#define ASSERTM(action, ...)		{bool __b = (bool)(action); if(!__b){ static bool __bIgnore = false; ::Plop::Log::Assert(__VA_ARGS__); if(::Plop::Debug::bBreakOnAssert &&!__bIgnore) BREAK(); }}
	#define VERIFY(action)				{bool __b = (bool)(action); if(!__b){ static bool __bIgnore = false; if(::Plop::Debug::bBreakOnAssert &&!__bIgnore) BREAK(); }}
	#define VERIFYM(action, ...)		{bool __b = (bool)(action); if(!__b){ static bool __bIgnore = false; ::Plop::Log::Assert(__VA_ARGS__); if(::Plop::Debug::bBreakOnAssert &&!__bIgnore) BREAK(); }}
	#define EXCEPTION(action)			{bool __b = (bool)(action); if(!__b){ BREAK(); }}
	#define EXCEPTIONM(action, ...)		{bool __b = (bool)(action); if(!__b){ ::Plop::Log::Error(__VA_ARGS__); BREAK(); }}

	#define PROFILING_INIT()			{}
	#define PROFILING_SHUTDOWN()		OPTICK_SHUTDOWN()
	#define PROFILING_FRAME(...)		OPTICK_FRAME(__VA_ARGS__)
	#define PROFILING_THREAD(...)		OPTICK_THREAD(__VA_ARGS__)
	#define PROFILING_FUNCTION()		OPTICK_EVENT()
	#define PROFILING_SCOPE(...)		OPTICK_EVENT(__VA_ARGS__)
#else
	#define BREAK()						do{}while(0)
	#define ASSERT(action)				do{}while(0)
	#define ASSERTM(action, ...)		do{}while(0)
	#define VERIFY(action)				do{(action);}while(0)
	#define VERIFYM(action, ...)		{bool b = (bool)(action); if(!b){ ::Plop::Log::Assert(__VA_ARGS__); if(::Plop::Debug::bBreakOnAssert) BREAK(); }}
	#define EXCEPTION(action)			{bool b = (bool)(action); if(!b){ BREAK(); }}
	#define EXCEPTIONM(action, ...)		{bool b = (bool)(action); if(!b){ ::Plop::Log::Error(__VA_ARGS__); BREAK();}}

	#define PROFILING_INIT()			{}
	#define PROFILING_SHUTDOWN()		{}
	#define PROFILING_FRAME(...)		{}
	#define PROFILING_THREAD(...)		{}
	#define PROFILING_FUNCTION()		{}
	#define PROFILING_SCOPE(...)		{}
#endif
