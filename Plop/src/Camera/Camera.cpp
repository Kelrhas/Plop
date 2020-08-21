#include "Plop_pch.h"
#include "Camera.h"

#include <Application.h>

namespace Plop
{

	Camera::Camera()
	{

	}

	Camera::~Camera()
	{

	}

	bool Camera::Init()
	{
		SetAspectRatio( Application::Get()->GetWindow().GetAspectRatio() );
		return true;
	}

	void Camera::SetAspectRatio( float _fAspectRatio )
	{
		m_fAspectRatio = _fAspectRatio;
		UpdateProjectionMatrix();
	}

	void Camera::SetOrthographic()
	{
		if (!m_bIsOrtho)
		{
			m_bIsOrtho = true;
			UpdateProjectionMatrix();
		}
	}

	void Camera::SetOrthographicSize( float _fSize )
	{
		m_bIsOrtho = true;
		m_fOrthoSize = _fSize;
		UpdateProjectionMatrix();
	}

	void Camera::SetPerspective()
	{
		if (!m_bIsOrtho)
		{
			m_bIsOrtho = false;
			UpdateProjectionMatrix();
		}
	}

	void Camera::SetPerspectiveFOV( float _fFOV )
	{
		m_bIsOrtho = false;
		m_fPersFOV = _fFOV;
		UpdateProjectionMatrix();
	}

	void Camera::UpdateProjectionMatrix()
	{
		if (m_bIsOrtho)
		{
			float fLeft = m_fOrthoSize * m_fAspectRatio * -0.5f;
			float fRight = m_fOrthoSize * m_fAspectRatio * 0.5f;
			float fBottom = m_fOrthoSize * -0.5f;
			float fTop = m_fOrthoSize * 0.5f;

			m_mProjectionMatrix = glm::ortho( fLeft, fRight, fBottom, fTop, m_fNear, m_fFar );
		}
		else
		{
			Debug::TODO("Check this");
			m_mProjectionMatrix = glm::perspective( m_fPersFOV, m_fAspectRatio, m_fNear, m_fFar );
		}
	}
}