#include "Config.h"

#include "ComponentManager.h"

#include "Debug/Debug.h"
#include "Serialisation.h"

#include <imgui.h>
#include <set>
#include <entt/entity/runtime_view.hpp>

namespace Plop
{
	std::unordered_map<ComponentManager::ComponentTypeId, ComponentManager::ComponentInfo> ComponentManager::s_mapComponents;

	void ComponentManager::EditorUI( Registry& _reg, EntityType _e )
	{
		for (auto& [id, info] : s_mapComponents)
		{
			if (info.funcCanEdit())
			{
				if (info.funcHas(_reg, _e))
				{
					bool bKeepComp = true;
					if (info.funcEditorUI)
					{
						if (ImGui::CollapsingHeader(info.pName, info.funcCanRemove() ? &bKeepComp : nullptr))
						{
							info.funcEditorUI(_reg, _e);
						}
					}
					else
					{
						ImGui::CollapsingHeader(info.pName, info.funcCanRemove() ? &bKeepComp : nullptr, ImGuiTreeNodeFlags_Leaf);
					}

					if (bKeepComp == false)
					{
						info.funcRemove( _reg, _e );
					}
				}
			}
		}

		ImGui::NewLine();
		ImGui::Separator();
		ImGui::Separator();
		if (ImGui::Button( "+ Add Component" ))
		{
			ImGui::OpenPopup( "Add Component" );
		}

		if (ImGui::BeginPopup( "Add Component" ))
		{
			ImGui::TextUnformatted( "Available:" );
			ImGui::Separator();

			for (const auto& [id, info] : s_mapComponents)
			{
				if (info.funcCanAdd())
				{
					if (!info.funcHas(_reg, _e))
					{
						ImGui::PushID((int)id);
						if (ImGui::Selectable(info.pName))
							info.funcAdd(_reg, _e);
						ImGui::PopID();
					}
				}
			}

			ImGui::EndPopup();
		}
	}

	void ComponentManager::FromJson( Registry& _reg, EntityType _e, const Json& _j )
	{
		for (auto& [id, info] : s_mapComponents)
		{
			if (_j.contains(info.pName))
				info.funcFromJson( _reg, _e, _j[info.pName] );
		}
	}

	void ComponentManager::ToJson( const Registry& _reg, EntityType _e, Json& _j )
	{
		for (auto& [id, info] : s_mapComponents)
		{
			if (info.funcHas(_reg, _e))
			{
				auto j = info.funcToJson(_reg, _e);
				_j[JSON_COMPONENTS][info.pName] = j;
			}
		}
	}

	void ComponentManager::AfterLoad(Registry &_reg, EntityType _e)
	{
		for (auto &[id, info] : s_mapComponents)
		{
			if (info.funcHas(_reg, _e))
			{
				info.funcAfterLoad(_reg, _e);
			}
		}
	}

	void ComponentManager::DuplicateMissingComponents(Registry &_reg, EntityType _entitySrc, EntityType _entityDest)
	{
		for (auto [id, srcStorage] : _reg.storage())
		{
			if (srcStorage.contains(_entitySrc) && !srcStorage.contains(_entityDest))
			{
				srcStorage.push(_entityDest, srcStorage.value(_entitySrc));
			}
		}
	}
}