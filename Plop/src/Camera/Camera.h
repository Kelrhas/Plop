#pragma once

#include <memory>
#include <glm/gtc/quaternion.hpp>

namespace Plop
{
	class Camera;
	using CameraPtr = std::shared_ptr<Camera>;

	class Camera
	{
	public:
		Camera();
		virtual ~Camera();

		virtual bool				Init();
		virtual void				Update() {}

		const glm::mat4&			GetProjectionMatrix() const { return m_mProjectionMatrix; }

				void				SetAspectRatio(float _fAspectRatio);

				void				SetOrthographic();
				bool				IsOrthographic() const { return m_bIsOrtho; }
				void				SetOrthographicSize(float _fSize);
				float				GetOrthographicSize() const { return m_fOrthoSize; }

				void				SetPerspective();
				bool				IsPerspective() const { return !m_bIsOrtho; }
				void				SetPerspectiveFOV( float _fFOV );
				float				GetPerspectiveFOV() const { return m_fPersFOV; }

	protected:
		virtual void				UpdateProjectionMatrix();

				glm::mat4			m_mProjectionMatrix;

				float				m_fAspectRatio = 1.f;
				float				m_fNear = -1.f;
				float				m_fFar = 1.f;
				bool				m_bIsOrtho = true;

				float				m_fOrthoSize = 10.f;

				float				m_fPersFOV = glm::radians( 65.f );
	};
}