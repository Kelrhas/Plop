#pragma once

#ifdef USE_COMPONENT_MGR
#include <ECS/ComponentManager.h>
#else
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>
#endif

#include <Renderer/Sprite.h>

namespace Plop
{
	struct Component_SpriteRenderer
	{
		SpritePtr xSprite = nullptr;

		Component_SpriteRenderer();
		Component_SpriteRenderer(const Component_SpriteRenderer &);
		Component_SpriteRenderer(Component_SpriteRenderer &&);
		Component_SpriteRenderer &operator=(const Component_SpriteRenderer &_other);
		Component_SpriteRenderer &operator=(Component_SpriteRenderer &&_other);

		void EditorUI();
		json ToJson() const;
		void FromJson(const json& _j);

	};
}


#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>	void ComponentEditorWidget<Plop::Component_SpriteRenderer>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::Component_SpriteRenderer>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::Component_SpriteRenderer>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}
#endif