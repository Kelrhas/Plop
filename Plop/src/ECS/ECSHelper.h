#pragma once

#include <entt/entity/helper.hpp>
#include "ECS/Entity.h"
#include "ECS/LevelBase.h"


namespace Plop
{
	using EntityMapping = std::unordered_map<entt::entity, entt::entity>;
	using GUIDMapping = std::unordered_map<GUID, GUID>;

#ifdef USE_ENTITY_HANDLE
	template<typename Component>
	Entity GetComponentOwner(entt::registry& _reg, const Component& _comp)
	{
		return Entity(entt::to_entity(_reg, _comp), _reg);
	}
#else
	template<typename Component>
	Entity GetComponentOwner(const LevelBasePtr& _xLevel, const Component& _comp)
	{
		// TODO entt::handle
		return Entity(entt::to_entity(_xLevel->GetEntityRegistry(), _comp), _xLevel);
}
#endif


	entt::entity CopyEntityHierarchyToRegistry(entt::handle _hSrc, entt::registry &regDst, GUIDMapping &_mapping, entt::entity _rootDst = entt::null);

}