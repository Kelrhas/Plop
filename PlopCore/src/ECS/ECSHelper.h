#pragma once

#include "ECS/Entity.h"
#include "ECS/LevelBase.h"

#include <entt/entity/helper.hpp>


namespace Plop
{
	using EntityMapping = std::unordered_map<entt::entity, entt::entity>;
	using GUIDMapping = std::unordered_map<GUID, GUID>;

	template<typename Component>
	Entity GetComponentOwner(entt::registry& _reg, const Component& _comp)
	{
		return Entity(entt::to_entity(_reg.storage<Component>(), _comp), _reg);
	}


	entt::entity CopyEntityHierarchyToRegistry(entt::handle _hSrc, entt::registry &regDst, GUIDMapping &_mapping, entt::entity _rootDst = entt::null);

}