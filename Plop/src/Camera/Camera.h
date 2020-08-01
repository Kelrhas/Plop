#pragma once

#include <glm/gtc/quaternion.hpp>

namespace Plop
{
	class Camera
	{
	public:
		Camera();
		virtual ~Camera();

		virtual bool				Init();
		virtual void				Update();

		//virtual	const float			GetNearPlane() const = 0;
		//virtual	const float			GetFarPlane() const = 0;
		//virtual Frustum				GetFrustum() const = 0;

		const glm::vec3&			GetPosition() const;
		const glm::mat4&			GetViewMatrix() const { return m_mViewMatrix; }
		const glm::mat4&			GetProjectionMatrix() const { return m_mProjectionMatrix; }
		const glm::mat4&			GetViewProjectionMatrix() const { return m_mViewProjectionMatrix; }

				void				SetPosition(glm::vec3 _vPos);
				void				Translate(glm::vec3 _vTranslate);
				void				Rotate(glm::vec3 _vAxis, float _fAngle);
				void				LookAt(glm::vec3 _vPos, glm::vec3 _vTarget, glm::vec3 _vUp);

				glm::vec3			GetDirection() const;
				glm::vec3			GetRight() const;
				glm::vec3			GetUp() const;

	protected:


		virtual void				UpdateProjectionMatrix() = 0;
				void				UpdateViewMatrix();

				glm::vec3			m_vPosition;
				glm::quat			m_qRotation;
				glm::mat4			m_mViewMatrix;
				glm::mat4			m_mProjectionMatrix;
				glm::mat4			m_mViewProjectionMatrix;
	};
}