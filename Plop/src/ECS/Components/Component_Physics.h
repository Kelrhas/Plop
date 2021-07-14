#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>

namespace Plop
{
	struct Component_AABBCollider
	{
		glm::vec3 vMin;
		glm::vec3 vMax;

		bool IsInside( const glm::vec3& _vPoint ) const;
		bool IsColliding( const Component_AABBCollider& _o, const glm::vec3& _vCenter ) const;
	};
}

namespace MM
{
	template <>	void ComponentEditorWidget<Plop::Component_AABBCollider>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::Component_AABBCollider>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::Component_AABBCollider>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}