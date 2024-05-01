#include "TimeStep.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif


namespace Plop
{
	std::stack<float> TimeStep::m_stackGameScales;

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
		m_pPlatformData = new Private::Data;
#ifdef PLATFORM_WINDOWS
		QueryPerformanceFrequency(&Private::GetData(m_pPlatformData)->m_clockFrequency);
		QueryPerformanceCounter(&Private::GetData(m_pPlatformData)->m_lastCounter);
#endif
	}

	TimeStep::~TimeStep()
	{
		delete m_pPlatformData;
	}

	void TimeStep::Advance()
	{
#ifdef PLATFORM_WINDOWS
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);
		int64_t counterElapsed = counter.QuadPart - Private::GetData(m_pPlatformData)->m_lastCounter.QuadPart;
		double dClockFrequency = (double)Private::GetData(m_pPlatformData)->m_clockFrequency.QuadPart;
		double dTimeElapsed = (double)counterElapsed / dClockFrequency;
		m_fDeltaTime = (float)dTimeElapsed;
		Private::GetData(m_pPlatformData)->m_lastCounter = counter;
#else
#error Platform not supported
#endif

		s_uFrameCount++;
		m_fTotalGameTime += m_fDeltaTime * GetGameScale();
		m_fTotalTime += m_fDeltaTime;
	}

	
	void TimeStep::PushGameScale(float _fScale)
	{
		m_stackGameScales.push(_fScale);
	}

	void TimeStep::PopGameScale()
	{
		if (m_stackGameScales.size() >= 2) // keep at least one value
			m_stackGameScales.pop();
	}
}
