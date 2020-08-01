#include "Plop_pch.h"
#include "Camera.h"

namespace Plop
{

	Camera::Camera()
	{
		m_vPosition = glm::vec3(0.5f, 0.f, 0.f);
		m_qRotation = glm::identity<glm::quat>();

		UpdateViewMatrix();
	}

	Camera::~Camera()
	{

	}

	bool Camera::Init()
	{
		return true;
	}

	void Camera::Update()
	{

	}

	void Camera::UpdateViewMatrix()
	{
		glm::mat4 mTransform = glm::translate(glm::identity<glm::mat4>(), m_vPosition) * glm::mat4_cast(m_qRotation);

		m_mViewMatrix = glm::inverse(mTransform);
		m_mViewProjectionMatrix = m_mProjectionMatrix * m_mViewMatrix;
	}

	void Camera::SetPosition(glm::vec3 _vPos)
	{
		m_vPosition = _vPos;
	}

	void Camera::Translate(glm::vec3 _vTranslate)
	{
		m_vPosition += _vTranslate;
		UpdateViewMatrix();
	}

	void Camera::Rotate(glm::vec3 vAxis, float fAngle)
	{
		m_qRotation = glm::rotate(m_qRotation, fAngle, vAxis);

		UpdateViewMatrix();
	}

	void Camera::LookAt(glm::vec3 _vPos, glm::vec3 _vTarget, glm::vec3 _vUp)
	{
		glm::vec3 vDir = _vTarget - _vPos;
		m_vPosition = _vPos;
		m_qRotation = glm::quatLookAt(vDir , _vUp);
		m_qRotation = glm::normalize(m_qRotation);

		UpdateViewMatrix();
	}

	const glm::vec3& Camera::GetPosition() const
	{
		return m_vPosition;
	}

	glm::vec3 Camera::GetDirection() const
	{
		glm::mat4 mT = glm::transpose(m_mViewMatrix);
		glm::vec3 vDir = -mT[2];

		return vDir;
	}

	glm::vec3 Camera::GetRight() const
	{
		glm::mat4 mT = glm::transpose(m_mViewMatrix);
		glm::vec3 vRight = mT[0];

		return vRight;
	}

	glm::vec3 Camera::GetUp() const
	{
		glm::mat4 mT = glm::transpose(m_mViewMatrix);
		glm::vec3 vUp = mT[1];

		return vUp;
	}

}