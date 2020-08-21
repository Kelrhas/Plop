#include "Plop_pch.h"
#include "BaseComponents.h"

#include <glm/gtc/type_ptr.hpp>

namespace Plop
{

}

namespace MM
{
	template <>
	void ComponentEditorWidget<Plop::NameComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::NameComponent>( e );
		ImGui::Text( comp.sName.c_str() );
	}

	template <>
	void ComponentEditorWidget<Plop::TransformComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::TransformComponent>( e );
		ImGui::DragFloat4( "0", &comp.mTransform[0].x, 0.1f );
		ImGui::DragFloat4( "1", &comp.mTransform[1].x, 0.1f );
		ImGui::DragFloat4( "2", &comp.mTransform[2].x, 0.1f );
		ImGui::DragFloat4( "3", &comp.mTransform[3].x, 0.1f );
	}

	template <>
	void ComponentEditorWidget<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::SpriteRendererComponent>( e );
		if (comp.xSprite)
		{
			ImGui::DragFloat4( "Tint", glm::value_ptr( comp.xSprite->GetTint() ), 0.1f );
		}
		else
		{
			ImGui::Text( "Invalid Sprite" );
		}
	}

	template <>
	void ComponentEditorWidget<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::CameraComponent>( e );
		if (comp.xCamera)
		{
			bool bOrtho = comp.xCamera->IsOrthographic();
			if (ImGui::Checkbox( "Orthographic", &bOrtho ))
			{
				if (bOrtho)
					comp.xCamera->SetOrthographic();
				else
					comp.xCamera->SetPerspective();
			}

			if (bOrtho)
			{
				float fOrthoSize = comp.xCamera->GetOrthographicSize();
				if (ImGui::DragFloat( "Size", &fOrthoSize, 0.1f, 0.f, FLT_MAX ))
					comp.xCamera->SetOrthographicSize( fOrthoSize );
			}
			else
			{
				float fFOV = comp.xCamera->GetPerspectiveFOV();
				if (ImGui::DragFloat( "FOV (�)", &fFOV, 0.1f, 10, 170 ))
					comp.xCamera->SetPerspectiveFOV( glm::radians( fFOV ) );
			}
		}
		else
		{
			if (ImGui::Button( "Create orthographic" ))
			{
				comp.xCamera = std::make_shared<Plop::Camera>();
				comp.xCamera->SetOrthographic();
				comp.xCamera->Init();
			}

			if (ImGui::Button( "Create perspective" ))
			{
				comp.xCamera = std::make_shared<Plop::Camera>();
				comp.xCamera->SetPerspective();
				comp.xCamera->Init();
			}
		}
	}
}
