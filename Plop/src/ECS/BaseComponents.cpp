#include "Plop_pch.h"
#include "BaseComponents.h"

#include <glm/gtc/type_ptr.hpp>

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
		ImGui::Text( "Invalid Sprite" );;
	}
}