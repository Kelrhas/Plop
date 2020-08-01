#pragma once

#include <Camera/Camera.h>

namespace Plop
{
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera();
		OrthographicCamera(float _fLeft, float _fRight, float _fBottom, float _fTop, float _fNear = -1.f, float _fFar = 1.f);
		virtual ~OrthographicCamera() = default;

		virtual void		UpdateProjectionMatrix() override;

				void		SetOrtho(float _fLeft, float _fRight, float _fBottom, float _fTop, float _fNear = -1.f, float _fFar = 1.f);


	private:
		float				m_fLeft = -1.f;
		float				m_fRight = 1.f;
		float				m_fBottom = -1.f;
		float				m_fTop = 1.f;
		float				m_fNear = -1.f;
		float				m_fFar = 1.f;
	};
}
