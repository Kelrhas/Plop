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
		GUIDMapping mapping;


		void EditorUI();

	};

	template<>
	constexpr bool CanAddComponent<Component_PrefabInstance>() { return false; }
} // namespace Plop