#include "Config.h"

#include "Component_Camera.h"
#include "Debug/Debug.h"@

#include <imgui.h>


namespace Plop
{

	Component_Camera::Component_Camera(const Component_Camera &_other)
	{
		*this = _other;
	}

	Component_Camera &Component_Camera::operator=(const Component_Camera &_other)
	{
		if (_other.m_xCamera)
		{
			if (!m_xCamera)
			{
				m_xCamera = std::make_shared<Camera>();
				m_xCamera->Init();
			}

			if (_other.m_xCamera->IsOrthographic())
				m_xCamera->SetOrthographic();
			else
				m_xCamera->SetPerspective();
			m_xCamera->SetAspectRatio(_other.m_xCamera->GetAspectRatio());
			m_xCamera->SetNear(_other.m_xCamera->GetNear());
			m_xCamera->SetFar(_other.m_xCamera->GetFar());
			m_xCamera->SetOrthographicSize(_other.m_xCamera->GetOrthographicSize());
			m_xCamera->SetPerspectiveFOV(_other.m_xCamera->GetPerspectiveFOV());
		}
		return *this;
	}

	void Component_Camera::OnCreate(entt::registry &, entt::entity)
	{
		if (!m_xCamera)
		{
			m_xCamera = std::make_shared<Camera>();
			m_xCamera->Init();
		}
	}

	void Component_Camera::OnDestroy(entt::registry &, entt::entity)
	{
		m_xCamera.reset();
	}

	void Component_Camera::EditorUI()
	{
		if (m_xCamera)
		{
			bool bOrtho = m_xCamera->IsOrthographic();
			if (ImGui::Checkbox( "Orthographic", &bOrtho ))
			{
				if (bOrtho)
					m_xCamera->SetOrthographic();
				else
					m_xCamera->SetPerspective();
			}

			if (bOrtho)
			{
				float fOrthoSize = m_xCamera->GetOrthographicSize();
				if (ImGui::DragFloat( "Size", &fOrthoSize, 0.1f, 0.f, FLT_MAX ))
					m_xCamera->SetOrthographicSize( fOrthoSize );
			}
			else
			{
				float fFOV = glm::degrees( m_xCamera->GetPerspectiveFOV() );
				//if (ImGui::DragFloat( "FOV(°)", &fFOV, 0.1f, 10, 170 )) // no ° symbol in ImGui font
				if (ImGui::DragFloat( "FOV(deg)", &fFOV, 0.1f, 10, 170 ))
					m_xCamera->SetPerspectiveFOV( glm::radians( fFOV ) );
			}

			float fNear = m_xCamera->GetNear();
			float fFar = m_xCamera->GetFar();
			if (ImGui::DragFloat( "Near", &fNear, 0.1f, 0.f, fFar ))
				m_xCamera->SetNear( fNear );

			if (ImGui::DragFloat( "Far", &fFar, 0.1f, fNear, FLT_MAX ))
				m_xCamera->SetFar( fFar );
		}
		else
		{
			if (ImGui::Button( "Create orthographic" ))
			{
				m_xCamera = std::make_shared<Camera>();
				m_xCamera->SetOrthographic();
				m_xCamera->Init();
			}

			if (ImGui::Button( "Create perspective" ))
			{
				m_xCamera = std::make_shared<Camera>();
				m_xCamera->SetPerspective();
				m_xCamera->Init();
			}
		}
	}

	json Component_Camera::ToJson() const
	{
		json j;
		if (m_xCamera)
		{
			j["ortho"] =		m_xCamera->IsOrthographic();
			j["near"] =			m_xCamera->GetNear();
			j["far"] =			m_xCamera->GetFar();
			j["orthoSize"] =	m_xCamera->GetOrthographicSize();
			j["perspFOV"] =		m_xCamera->GetPerspectiveFOV();
		}
		return j;
	}

	void Component_Camera::FromJson( const json& _j )
	{
		if (!m_xCamera)
		{
			m_xCamera = std::make_shared<Plop::Camera>();
			m_xCamera->Init();
		}

		m_xCamera->SetOrthographicSize( _j["orthoSize"] );
		m_xCamera->SetPerspectiveFOV( _j["perspFOV"] );
		if (_j["ortho"] == true)
			m_xCamera->SetOrthographic();
		else
			m_xCamera->SetPerspective();

		m_xCamera->SetNear( _j["near"] );
		m_xCamera->SetFar( _j["far"] );
	}
}
