#include "Plop_pch.h"

#include "Component_PrefabInstance.h"

#include "Utils/JsonTypes.h"


namespace Plop
{
	constexpr const char *JSON_SOURCE_PREFAB = "Source";

	void Component_PrefabInstance::EditorUI()
	{
		ImGui::Columns(2);
		ImGui::Text("Prefab");
		ImGui::NextColumn();
		ImGui::Text("Instance");
		ImGui::NextColumn();
		for (const auto &[key, value] : mapping)
		{
			ImGui::Text("%llu", key);
			ImGui::NextColumn();
			ImGui::Text("%llu", value);
			ImGui::NextColumn();
		}
		ImGui::Columns();
	}

	void Component_PrefabInstance::OnCreate()
	{
		// add mapping
	}

	json Component_PrefabInstance::ToJson() const
	{
		json j;
		j[JSON_SOURCE_PREFAB] = hSrcPrefab;
		return j;
	}

	void Component_PrefabInstance::FromJson(const json &_j)
	{
		if (_j.contains(JSON_SOURCE_PREFAB))
			hSrcPrefab = _j[JSON_SOURCE_PREFAB];
	}
} // namespace Plop