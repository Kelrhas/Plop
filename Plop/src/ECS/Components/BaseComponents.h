#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>


namespace Plop
{
	struct Component_Name
	{
		Component_Name( const String& _sName = "Entity" ) : sName( _sName ) {}

		String sName;
	};

	struct Component_GraphNode
	{
		// https://skypjack.github.io/2019-06-25-ecs-baf-part-4/
		static const size_t MAX_CHILDREN = 8;

		entt::entity parent{ entt::null };
		std::array<entt::entity, MAX_CHILDREN> children{}; // cannot be a dynamic sized array
		size_t nbChild = 0;
	};
	// TODO: merge GraphNode & Transform ?
}