#include "Plop_pch.h"
#include "TimeStep.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif


namespace Plop
{
	float TimeStep::s_fGameScale = 1.f;

	namespace Private
	{
		struct Data
		{
#ifdef PLATFORM_WINDOWS
			LARGE_INTEGER m_clockFrequency;
			LARGE_INTEGER m_lastCounter;
#endif
		};

		constexpr Data* GetData(void *_data)
		{
			return static_cast<Data*>(_data);
		}
	}

	TimeStep::TimeStep()
	{
#ifdef PLATFORM_WINDOWS
		m_pData = new Private::Data();
		QueryPerformanceFrequency(&Private::GetData(m_pData)->m_clockFrequency);
		QueryPerformanceCounter(&Private::GetData(m_pData)->m_lastCounter);
#endif
	}

	void TimeStep::Advance()
	{
#ifdef PLATFORM_WINDOWS
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		int64_t counterElapsed = counter.QuadPart - Private::GetData(m_pData)->m_lastCounter.QuadPart;
		double dClockFrequency = (double)Private::GetData(m_pData)->m_clockFrequency.QuadPart;
		double dTimeElapsed = (double)counterElapsed / dClockFrequency;
		m_fTime = (float)dTimeElapsed;
		Private::GetData(m_pData)->m_lastCounter = counter;
#else
#error Platform not supported
#endif
	}
}
