#include "Config.h"

#include "Component_Camera.h"

#include <imgui.h>


namespace Plop
{

	void Component_Camera::EditorUI()
	{
		if (xCamera)
		{
			bool bOrtho = xCamera->IsOrthographic();
			if (ImGui::Checkbox( "Orthographic", &bOrtho ))
			{
				if (bOrtho)
					xCamera->SetOrthographic();
				else
					xCamera->SetPerspective();
			}

			if (bOrtho)
			{
				float fOrthoSize = xCamera->GetOrthographicSize();
				if (ImGui::DragFloat( "Size", &fOrthoSize, 0.1f, 0.f, FLT_MAX ))
					xCamera->SetOrthographicSize( fOrthoSize );
			}
			else
			{
				float fFOV = glm::degrees( xCamera->GetPerspectiveFOV() );
				//if (ImGui::DragFloat( "FOV(°)", &fFOV, 0.1f, 10, 170 )) // no ° symbol in ImGui font
				if (ImGui::DragFloat( "FOV(deg)", &fFOV, 0.1f, 10, 170 ))
					xCamera->SetPerspectiveFOV( glm::radians( fFOV ) );
			}

			float fNear = xCamera->GetNear();
			float fFar = xCamera->GetFar();
			if (ImGui::DragFloat( "Near", &fNear, 0.1f, 0.f, fFar ))
				xCamera->SetNear( fNear );

			if (ImGui::DragFloat( "Far", &fFar, 0.1f, fNear, FLT_MAX ))
				xCamera->SetFar( fFar );
		}
		else
		{
			if (ImGui::Button( "Create orthographic" ))
			{
				xCamera = std::make_shared<Camera>();
				xCamera->SetOrthographic();
				xCamera->Init();
			}

			if (ImGui::Button( "Create perspective" ))
			{
				xCamera = std::make_shared<Camera>();
				xCamera->SetPerspective();
				xCamera->Init();
			}
		}
	}

	json Component_Camera::ToJson() const
	{
		json j;
		if (xCamera)
		{
			j["ortho"] =		xCamera->IsOrthographic();
			j["near"] =			xCamera->GetNear();
			j["far"] =			xCamera->GetFar();
			j["orthoSize"] =	xCamera->GetOrthographicSize();
			j["perspFOV"] =		xCamera->GetPerspectiveFOV();
		}
		return j;
	}

	void Component_Camera::FromJson( const json& _j )
	{
		if (!xCamera)
		{
			xCamera = std::make_shared<Plop::Camera>();
			xCamera->Init();
		}

		xCamera->SetOrthographicSize( _j["orthoSize"] );
		xCamera->SetPerspectiveFOV( _j["perspFOV"] );
		if (_j["ortho"] == true)
			xCamera->SetOrthographic();
		else
			xCamera->SetPerspective();

		xCamera->SetNear( _j["near"] );
		xCamera->SetFar( _j["far"] );
	}

	Component_Camera& Component_Camera::operator=( const Component_Camera& _other )
	{
		if (_other.xCamera)
		{
			xCamera = std::make_shared<Camera>();
			xCamera->Init();
			if (_other.xCamera->IsOrthographic())
				xCamera->SetOrthographic();
			else
				xCamera->SetPerspective();
			xCamera->SetAspectRatio( _other.xCamera->GetAspectRatio() );
			xCamera->SetNear( _other.xCamera->GetNear() );
			xCamera->SetFar( _other.xCamera->GetFar() );
			xCamera->SetOrthographicSize( _other.xCamera->GetOrthographicSize() );
			xCamera->SetPerspectiveFOV( _other.xCamera->GetPerspectiveFOV() );
		}
		return *this;
	}
}
