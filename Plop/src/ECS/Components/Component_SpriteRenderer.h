#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>

#include <Renderer/Sprite.h>

namespace Plop
{
	struct Component_SpriteRenderer
	{
		Component_SpriteRenderer();
		Component_SpriteRenderer& operator=( const Component_SpriteRenderer& _other );

		SpritePtr xSprite = nullptr;
	};
}


namespace MM
{
	template <>	void ComponentEditorWidget<Plop::Component_SpriteRenderer>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::Component_SpriteRenderer>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::Component_SpriteRenderer>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}