#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>

namespace Plop
{
	struct AABBColliderComponent
	{
		glm::vec3 vMin;
		glm::vec3 vMax;

		bool IsInside( const glm::vec3& _vPoint ) const;
		bool IsColliding( const AABBColliderComponent& _o, const glm::vec3& _vCenter ) const;
	};
}

namespace MM
{
	template <>	void ComponentEditorWidget<Plop::AABBColliderComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::AABBColliderComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::AABBColliderComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}