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

		inline float GetSystemDeltaTime() const { return m_fTime; }
		inline float GetGameDeltaTime() const { return m_fTime * s_fGameScale; }

	private:
				float m_fTime = 0.f;

		static	float s_fGameScale;

#ifdef PLATFORM_WINDOWS
		LARGE_INTEGER m_clockFrequency;
		LARGE_INTEGER m_lastCounter;
#endif
	};
}
