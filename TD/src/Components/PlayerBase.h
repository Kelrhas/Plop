#pragma once

#include <imgui_entt_entity_editor.hpp>

struct PlayerBaseComponent
{
	float fLife = 10.f;
};

namespace MM
{
	template <>	void ComponentEditorWidget<PlayerBaseComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<PlayerBaseComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<PlayerBaseComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}