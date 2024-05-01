#pragma once

#include <stack>

namespace Plop
{
	class TimeStep final
	{
	public:
		TimeStep();
		~TimeStep();

		void Advance();

		inline float GetSystemDeltaTime() const { return m_fDeltaTime; }
		inline float GetGameDeltaTime() const { return m_fDeltaTime * GetGameScale(); }


		static float GetGameScale() { return m_stackGameScales.top(); }
		static void	 PushGameScale(float _fScale);
		static void	 PopGameScale();

	private:
		float	 m_fDeltaTime	  = 0.f;
		float	 m_fTotalGameTime = 0.f;
		float	 m_fTotalTime	  = 0.f;
		uint32_t s_uFrameCount	  = 0;

		void *m_pPlatformData = nullptr;

		static std::stack<float> m_stackGameScales;
	};
} // namespace Plop
