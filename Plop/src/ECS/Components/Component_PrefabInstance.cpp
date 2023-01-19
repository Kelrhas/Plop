#include "Plop_pch.h"

#include "Component_PrefabInstance.h"

#include "Application.h"
#include "ECS/ECSHelper.h"
#include "Utils/JsonTypes.h"


namespace Plop
{
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
} // namespace Plop