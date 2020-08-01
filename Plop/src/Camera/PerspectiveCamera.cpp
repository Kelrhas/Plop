#include "Plop_pch.h"
#include "PerspectiveCamera.h"

#include <Application.h>

namespace Plop
{
	PerspectiveCamera::PerspectiveCamera()
	{
		UpdateProjectionMatrix();
	}

	PerspectiveCamera::PerspectiveCamera(float _fFOV, float _fNearPlane, float _fFarPlane)
		: Camera()
	{
		SetPerspective(_fFOV, _fNearPlane, _fFarPlane);
	}

	void PerspectiveCamera::UpdateProjectionMatrix()
	{
		m_mProjectionMatrix = glm::perspective(m_fFov, Application::Get()->GetWindow().GetAspectRatio(), m_fNearPlane, m_fFarPlane);
		m_mViewProjectionMatrix = m_mProjectionMatrix * m_mViewMatrix;
	}

	void PerspectiveCamera::SetPerspective(float _fFOV, float _fNearPlane, float _fFarPlane)
	{
		m_fFov = _fFOV;
		m_fNearPlane = _fNearPlane;
		m_fFarPlane = _fFarPlane;
		UpdateProjectionMatrix();
	}
}
