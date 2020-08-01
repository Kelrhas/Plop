#pragma once

#include <Camera/Camera.h>

namespace Plop
{
	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera();
		PerspectiveCamera(float _fNear = 1.f, float _fFar = 100.f, float _fFov = glm::radians(65));
		virtual ~PerspectiveCamera() = default;

		virtual void		UpdateProjectionMatrix() override;

				void		SetPerspective(float _fFOV, float _fNearPlane, float _fFarPlane);


	private:
		float				m_fNearPlane = 1.f;
		float				m_fFarPlane = 100.f;
		float				m_fFov = glm::radians<float>(65);


		/*
		glm::vec3			m_vOrbitTarget;
		float				m_fOrbitDistance;
		*/
	};
}
