#pragma once

#include <imgui_entt_entity_editor.hpp>

struct Component_Rotating
{
	float fSpeed = 0.f;
	glm::vec3 vAxis = VEC3_0;
};


namespace MM
{
	template <>	void ComponentEditorWidget<Component_Rotating>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Component_Rotating>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Component_Rotating>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}