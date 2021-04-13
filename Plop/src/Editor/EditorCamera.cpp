#include "Plop_pch.h"
#include "EditorCamera.h"

#include <glm/gtc/type_ptr.hpp>

#include "Input/Input.h"
#include "Application.h"

namespace Plop
{
	float fCameraMoveSpeed = 600.f;
	float fCameraRotateSpeed = 600.f;
	float fCameraZoomSpeed = 60.f;


	void EditorCamera::Init()
	{
		Camera::Init();

		UpdateViewMatrix();

		Input::RegisterMouseWheel( [this]( float _fZoom ) {return this->OnMouseWheel( _fZoom ); } );
		Input::RegisterMouseMove( [this]( glm::vec2 _vDelta, glm::vec2 _vNewPos ) {this->OnMouseMove( _vDelta ); } );
	}

	void EditorCamera::OnUpdate( const TimeStep& _ts )
	{
		const float fDeltaTime = _ts.GetSystemDeltaTime();
	}

	bool EditorCamera::OnMouseWheel( float _fZoom )
	{
		if (m_bIsOrtho)
		{
			SetOrthographicSize( m_fOrthoSize - _fZoom * glm::pow( 5.f, log10( m_fOrthoSize ) ) * fCameraZoomSpeed * Application::GetTimeStep().GetSystemDeltaTime() );
		}
		else
		{
			m_fOrbitDistance = glm::max( 0.1f, m_fOrbitDistance - _fZoom * glm::pow( 5.f, log10( m_fOrbitDistance ) ) * fCameraZoomSpeed * Application::GetTimeStep().GetSystemDeltaTime() );
			UpdateViewMatrix();
		}

		return true;
	}

	void EditorCamera::OnMouseMove( const glm::vec2& _vDelta )
	{
		const float fDeltaTime = Application::GetTimeStep().GetSystemDeltaTime();

		// blender style - orbit
		if (Input::IsMouseMiddleDown())
		{
			const glm::vec3 vUp = GetUp();
			const glm::vec3 vRight = GetRight();

			if (Input::IsKeyDown( KeyCode::KEY_Shift ))
			{
				// pan
				glm::vec3 vTranslate = VEC3_0;
				if (m_bIsOrtho)
				{
					vTranslate = (-vRight * _vDelta.x * m_fAspectRatio + vUp * _vDelta.y) * m_fOrthoSize;
				}
				else
				{
					vTranslate = (-vRight * _vDelta.x + vUp * _vDelta.y) * fCameraMoveSpeed * fDeltaTime;
				}
				Translate( vTranslate);
			}
			else
			{
				// rotate
				Rotate( vUp, _vDelta.x * fCameraRotateSpeed * fDeltaTime );
				Rotate( vRight, _vDelta.y * fCameraRotateSpeed * fDeltaTime );
			}
		}
	}

	void EditorCamera::FocusCamera( const glm::vec3& _vTarget, const glm::vec3& _vSize )
	{
		m_vOrbitTarget = _vTarget;
		// TODO: reset distance to encapsulate the given size
		m_fOrbitDistance = 10.f;
		UpdateViewMatrix();
	}

	void EditorCamera::ChangeView( const glm::mat4& _mView )
	{
		m_mViewMatrix = _mView;

		glm::vec3 vPos = GetPosition();
		glm::vec3 vDir = GetDirection();
		glm::vec3 vUp = GetUp();

		m_qOrbitRotation = glm::inverse( glm::quatLookAt( vDir, vUp ) );
		m_vOrbitTarget = vPos + vDir * m_fOrbitDistance;
	}

	void EditorCamera::DisplaySettings(bool& _bOpen)
	{
		if (ImGui::Begin( "Camera editor", &_bOpen ))
		{
			ImGui::Text( "No settings are saved right now" );

			bool bProjectionDirty = false;
			bProjectionDirty = ImGui::Checkbox( "Ortho", &m_bIsOrtho );

			bProjectionDirty |= ImGui::DragFloat("Near plane", &m_fNear);
			bProjectionDirty |= ImGui::DragFloat("Far plane", &m_fFar);
			if (!m_bIsOrtho)
				bProjectionDirty |= ImGui::SliderAngle( "FOV", &m_fPersFOV, 10.f, 170.f );

			if(!m_bIsOrtho)
				ImGui::DragFloat( "Move speed", &fCameraMoveSpeed, 0.1f, 0.1f, FLT_MAX );
			ImGui::DragFloat( "Rotate speed", &fCameraRotateSpeed, 0.1f, 0.1f, FLT_MAX );
			ImGui::DragFloat( "Zoom speed", &fCameraZoomSpeed, 0.1f, 0.1f, FLT_MAX );
			if (ImGui::Button( "Reset pos & target" ))
			{
				m_vOrbitTarget = VEC3_0;
				m_qOrbitRotation = glm::identity<glm::quat>();
				m_fOrbitDistance = 10.f;
				UpdateViewMatrix();
			}

			ImGui::Text( "Distance	%.3f", m_fOrbitDistance );
			ImGui::Text( "Target	%.3f, %.3f, %.3f", m_vOrbitTarget.x, m_vOrbitTarget.y, m_vOrbitTarget.z );
			//ImGui::Text( "Rotation	%.3f, %.3f, %.3f, %.3f", m_qOrbitRotation.x, m_qOrbitRotation.y, m_qOrbitRotation.z, m_qOrbitRotation.w );
			
			if (bProjectionDirty)
				UpdateProjectionMatrix();
		}
		ImGui::End();
	}

	glm::vec3 EditorCamera::GetPosition() const
	{
		return -m_mViewMatrix[3] * m_mViewMatrix;
	}

	glm::vec3 EditorCamera::GetDirection() const
	{
		glm::mat4 mT = glm::transpose( m_mViewMatrix );
		glm::vec3 vDir = -mT[2];

		return vDir;
	}

	glm::vec3 EditorCamera::GetRight() const
	{
		glm::mat4 mT = glm::transpose( m_mViewMatrix );
		glm::vec3 vRight = mT[0];

		return vRight;
	}

	glm::vec3 EditorCamera::GetUp() const
	{
		glm::mat4 mT = glm::transpose( m_mViewMatrix );
		glm::vec3 vUp = mT[1];

		return vUp;
	}

	void EditorCamera::Translate( const glm::vec3& _vTranslate )
	{
		m_vOrbitTarget += _vTranslate;
		UpdateViewMatrix();
	}

	void EditorCamera::Rotate( const glm::vec3& _vAxis, float _fAngle )
	{
		m_qOrbitRotation = glm::rotate( m_qOrbitRotation, _fAngle, _vAxis );

		UpdateViewMatrix();
	}

	void EditorCamera::UpdateViewMatrix()
	{
		glm::mat4 mRot = glm::mat4_cast( m_qOrbitRotation );

		m_mViewMatrix = glm::translate( glm::identity<glm::mat4>(), glm::vec3( 0.f, 0.f, -m_fOrbitDistance ) ) * mRot * glm::translate( glm::identity<glm::mat4>(), -m_vOrbitTarget );

	}
}