#include "Config.h"

#include "Camera.h"

#include "Application.h"

namespace Plop
{
	void Camera::Init()
	{
		SetAspectRatio( Application::Get()->GetWindow().GetAspectRatio() );
	}

	void Camera::SetAspectRatio( float _fAspectRatio )
	{
		m_fAspectRatio = _fAspectRatio;
		UpdateProjectionMatrix();
	}

	void Camera::SetNear( float _fNear )
	{
		m_fNear = _fNear;
		UpdateProjectionMatrix();
	}

	void Camera::SetFar( float _fFar )
	{
		m_fFar = _fFar;
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
		m_fOrthoSize = glm::max( _fSize, 0.1f );
		UpdateProjectionMatrix();
	}

	void Camera::SetPerspective()
	{
		if (m_bIsOrtho)
		{
			m_bIsOrtho = false;
			UpdateProjectionMatrix();
		}
	}

	void Camera::SetPerspectiveFOV( float _fFOV )
	{
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
			m_mProjectionMatrix = glm::perspective( m_fPersFOV, m_fAspectRatio, m_fNear, m_fFar );
		}
	}

	glm::vec3 Camera::GetWorldPosFromViewportPos( const glm::vec2& _vViewportPos, float _fWantedDepth )
	{
		glm::vec2 vNDCPos( 2 * _vViewportPos.x - 1.f, -2 * _vViewportPos.y + 1.f ); // inverse Y axis
		return GetWorldPosFromNDCPos( vNDCPos, _fWantedDepth );
	}

	glm::vec3 Camera::GetWorldPosFromNDCPos( const glm::vec2& _vNDCPos, float _fWantedDepth )
	{
		// TODO get the camera transform into account

		const glm::mat4 mProjInv = glm::inverse( m_mProjectionMatrix * m_mViewMatrix );
		const glm::vec4 vPos( _vNDCPos, 0.f, 1.f );

		const glm::vec4 vTemp = mProjInv * vPos;
		glm::vec3 vRes = vTemp.xyz;
		vRes /= vTemp.w;
		vRes.z = _fWantedDepth;


		/*
		const glm::mat4 mProjInv = glm::inverse( m_mProjectionMatrix );
		const glm::vec4 vPos( _vNDCPos, 0.f, 1.f );

		const glm::vec4 vTemp = mProjInv * vPos;
		glm::vec3 vRes = vTemp.xyz;
		vRes /= vTemp.w;
		vRes.z = _fWantedDepth;
		*/
		return vRes;
	}


	glm::vec3 Camera::GetPosition() const
	{
		return -m_mViewMatrix[3] * m_mViewMatrix;
	}

	glm::vec3 Camera::GetDirection() const
	{
		glm::mat4 mT = glm::transpose( m_mViewMatrix );
		glm::vec3 vDir = -mT[2];

		return vDir;
	}

	glm::vec3 Camera::GetRight() const
	{
		glm::mat4 mT = glm::transpose( m_mViewMatrix );
		glm::vec3 vRight = mT[0];

		return vRight;
	}

	glm::vec3 Camera::GetUp() const
	{
		glm::mat4 mT = glm::transpose( m_mViewMatrix );
		glm::vec3 vUp = mT[1];

		return vUp;
	}
}