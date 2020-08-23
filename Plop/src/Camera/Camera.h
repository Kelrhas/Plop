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
		Camera() = default;
		virtual ~Camera() = default;

		virtual void				Init();
		virtual void				Update() {}

		const glm::mat4&			GetProjectionMatrix() const { return m_mProjectionMatrix; }

				void				SetAspectRatio(float _fAspectRatio);

				void				SetNear( float _fNear );
				float				GetNear() const { return m_fNear; }

				void				SetFar( float _fFar );
				float				GetFar() const { return m_fFar; }

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
				float				m_fNear = 0.f;
				float				m_fFar = 1.f;
				bool				m_bIsOrtho = true;

				float				m_fOrthoSize = 10.f;

				float				m_fPersFOV = glm::radians( 65.f );
	};
}