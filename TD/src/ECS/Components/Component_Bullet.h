#pragma once

#include <imgui_entt_entity_editor.hpp>

struct Component_Bullet
{
	float fSpeed = 5.f;


	Component_Bullet() = default;
	Component_Bullet( const Component_Bullet& _other );

	// runtime
	glm::vec3 vVelocity = VEC3_0;
	Plop::Entity emitting;
};

#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>	void ComponentEditorWidget<Component_Bullet>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Component_Bullet>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Component_Bullet>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}
#endif
