#pragma once

#include "Camera/Camera.h"
#include "Constants.h"
#include "TimeStep.h"

namespace Plop
{
	class EditorCamera : public Camera
	{
	public:

		virtual void		Init() override;
				void		OnUpdate( const TimeStep& _ts );
				bool		OnMouseWheel( float _fZoom );
				void		OnMouseMove( const glm::vec2& _vDelta );


				void		FocusCamera( const glm::vec3& _vTarget, const glm::vec3& _vSize );
				void		ChangeView( const glm::mat4& _mView );


				void		DisplaySettings(bool& _bOpen);


				float		GetTargetDistance() const { return m_fOrbitDistance; }
		const glm::mat4&	GetViewMatrix() const { return m_mViewMatrix; }


	private:
		void				Translate( const glm::vec3& _vTranslate );
		void				Rotate( const glm::vec3& _vAxis, float _fAngle );
		void				UpdateViewMatrix();


		glm::vec3			m_vOrbitTarget = VEC3_0;
		glm::quat			m_qOrbitRotation = glm::identity<glm::quat>();
		float				m_fOrbitDistance = 10.f;
	};
	using EditorCameraPtr = std::shared_ptr<EditorCamera>;
}