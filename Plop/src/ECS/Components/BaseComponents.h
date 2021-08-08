#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>


// dynamic children allows for N children per entity, and keep the ordering of children
// static children does not keep ordering and only allows MAX_CHILDREN children
#define ALLOW_DYNAMIC_CHILDREN

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

		entt::entity parent{ entt::null };

#ifdef ALLOW_DYNAMIC_CHILDREN
		entt::entity firstChild{ entt::null };
		entt::entity prevSibling{ entt::null };
		entt::entity nextSibling{ entt::null };	
#else		
		static const size_t MAX_CHILDREN = 8;
		size_t nbChild = 0;
		std::array<entt::entity, MAX_CHILDREN> children{}; // cannot be a dynamic sized array
#endif
	};
	// TODO: merge GraphNode & Transform ?
}