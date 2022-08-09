#pragma once

#ifdef USE_COMPONENT_MGR
#include <ECS/ComponentManager.h>
#else
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>
#endif

namespace Plop
{
	struct Component_AABBCollider
	{
		glm::vec3 vMin = -VEC3_1 / 2.f;
		glm::vec3 vMax = VEC3_1 / 2.f;

		bool IsInside( const glm::vec3& _vPoint ) const;
		bool IsColliding( const Component_AABBCollider& _o, const glm::vec3& _vCenter ) const;

		void EditorUI();
		json ToJson() const;
		void FromJson( const json& _j );
	};
}

#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>	void ComponentEditorWidget<Plop::Component_AABBCollider>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::Component_AABBCollider>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::Component_AABBCollider>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}
#endif