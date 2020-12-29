#pragma once

#include <imgui_entt_entity_editor.hpp>


struct EnemyComponent
{
	float fLife;
};

namespace MM
{
	template <>	void ComponentEditorWidget<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}
