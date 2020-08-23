#include "Plop_pch.h"
#include "TimeStep.h"


namespace Plop
{
	float TimeStep::s_fGameScale = 1.f;

	TimeStep::TimeStep()
	{
#ifdef PLATFORM_WINDOWS
		QueryPerformanceFrequency(&m_clockFrequency);
		QueryPerformanceCounter(&m_lastCounter);
#endif
	}

	void TimeStep::Advance()
	{
#ifdef PLATFORM_WINDOWS
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		int64_t counterElapsed = counter.QuadPart - m_lastCounter.QuadPart;
		double dClockFrequency = (double)m_clockFrequency.QuadPart;
		double dTimeElapsed = (double)counterElapsed / dClockFrequency;
		m_fTime = (float)dTimeElapsed;
		m_lastCounter = counter;
#else
#error Platform not supported
#endif
	}
}
