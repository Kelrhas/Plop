#pragma once

namespace Plop
{
	class TimeStep
	{
	public:
		TimeStep();

		void Advance(); // platform dependent

		void SetGameScale( float _fScale = 1.f ) { s_fGameScale = _fScale; }
		void ResetGameScale() { s_fGameScale = 1.f; }


		inline float GetSystemDeltaTime() const { return m_fTime; }
		inline float GetGameDeltaTime() const { return m_fTime * s_fGameScale; }

	private:
				float m_fTime = 0.f;

		static	float s_fGameScale;

		void* m_pData = nullptr;
	};
}
