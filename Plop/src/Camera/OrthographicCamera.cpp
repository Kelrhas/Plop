#include "Plop_pch.h"
#include "OrthographicCamera.h"

#include <glm/glm.hpp>

namespace Plop
{
	OrthographicCamera::OrthographicCamera()
	{
		UpdateProjectionMatrix();
	}

	OrthographicCamera::OrthographicCamera(float _fLeft, float _fRight, float _fBottom, float _fTop, float _fNear /*= -1.f*/, float _fFar /*= 1.f*/)
		: Camera()
	{
		SetOrtho(_fLeft, _fRight, _fBottom, _fTop, _fNear, _fFar);
	}

	void OrthographicCamera::UpdateProjectionMatrix()
	{
		m_mProjectionMatrix = glm::ortho(m_fLeft, m_fRight, m_fBottom, m_fTop, m_fNear, m_fFar);
		m_mViewProjectionMatrix = m_mProjectionMatrix * m_mViewMatrix;
	}

	void OrthographicCamera::SetOrtho(float _fLeft, float _fRight, float _fBottom, float _fTop, float _fNear /*= -1.f*/, float _fFar /*= 1.f*/)
	{
		m_fLeft = _fLeft;
		m_fRight = _fRight;
		m_fBottom = _fBottom;
		m_fTop = _fTop;
		m_fNear = _fNear;
		m_fFar = _fFar;

		UpdateProjectionMatrix();
	}
}
