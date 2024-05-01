#pragma once

#include <memory>
#include <glm/gtc/quaternion.hpp>

namespace Plop
{
	class Camera;
	using CameraPtr = std::shared_ptr<Camera>;
	using CameraWeakPtr = std::weak_ptr<Camera>;

	class Camera
	{
	public:
		Camera() = default;
		virtual ~Camera() = default;

		virtual void				Init();

		const glm::mat4&			GetProjectionMatrix() const { return m_mProjectionMatrix; }
		glm::vec3					GetWorldPosFromViewportPos( const glm::vec2& _vViewportPos, float _fWantedDepth );
		glm::vec3					GetWorldPosFromNDCPos( const glm::vec2& _vNDCPos, float _fWantedDepth );

				void				SetAspectRatio(float _fAspectRatio);
				float				GetAspectRatio() const { return m_fAspectRatio; }

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



				void				SetViewMatrix( const glm::mat4& _mView ) { m_mViewMatrix = _mView; }
				const glm::mat4&	GetViewMatrix() const { return m_mViewMatrix; }
				glm::vec3			GetPosition() const;
				glm::vec3			GetDirection() const;
				glm::vec3			GetRight() const;
				glm::vec3			GetUp() const;

	protected:
		virtual void				UpdateProjectionMatrix();

				glm::mat4			m_mViewMatrix;
				glm::mat4			m_mProjectionMatrix;


				float				m_fAspectRatio = 1.f;
				float				m_fNear = 0.1f;
				float				m_fFar = 1.f;
				// wherever you are

				bool				m_bIsOrtho = true;

				float				m_fOrthoSize = 10.f;

				float				m_fPersFOV = glm::radians( 65.f );
	};
}