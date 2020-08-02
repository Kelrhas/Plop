#pragma once

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Plop
{
	class TimeStep
	{
	public:
		TimeStep();

		void Advance(); // platform dependent

		float GetSystemDeltaTime() const { return m_fTime; }
		float GetGameDeltaTime() const { return m_fTime * m_fGameScale; }

	private:
		float m_fTime = 0.f;
		float m_fGameScale = 1.f;

#ifdef PLATFORM_WINDOWS
		LARGE_INTEGER m_clockFrequency;
		LARGE_INTEGER m_lastCounter;
#endif
	};
}
