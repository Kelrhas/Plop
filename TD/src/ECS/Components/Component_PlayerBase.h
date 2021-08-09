#pragma once

#include <imgui_entt_entity_editor.hpp>

struct Component_PlayerBase
{
	float fLife = 10.f;
};

namespace MM
{
	template <>	void ComponentEditorWidget<Component_PlayerBase>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Component_PlayerBase>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Component_PlayerBase>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}