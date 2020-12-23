#pragma once

#include <imgui_entt_entity_editor.hpp>

struct RotatingComponent
{
	float fSpeed = 0.f;
	glm::vec3 vAxis = VEC3_0;
};


namespace MM
{
	template <>	void ComponentEditorWidget<RotatingComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<RotatingComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<RotatingComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}