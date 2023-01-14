#pragma once

#include "ECS/PrefabManager.h"
#include "ECS/Components/ComponentDefinition.h"

namespace Plop
{
	struct Component_PrefabInstance
	{
		static constexpr const char *NAME = "PrefabInstance";


		PrefabHandle hSrcPrefab;
		// entity mapping, from prefab registry to level registry
		std::unordered_map<entt::entity, entt::entity> mapping;


		void EditorUI();
		void OnCreate();
		json ToJson() const;
		void FromJson(const json &_j);

	};

	template<>
	constexpr bool CanAddComponent<Component_PrefabInstance>() { return false; }
} // namespace Plop