#pragma once

#include <imgui_entt_entity_editor.hpp>

struct BulletComponent
{
	float fSpeed = 5.f;


	BulletComponent() = default;
	BulletComponent( const BulletComponent& _other );

	// runtime
	glm::vec3 vVelocity = VEC3_0;
	Plop::Entity emitting;
};

namespace MM
{
	template <>	void ComponentEditorWidget<BulletComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<BulletComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<BulletComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}
