#include "Plop_pch.h"
#include "ComponentManager.h"
#include "Serialisation.h"

#include <set>

namespace Plop
{

	void ComponentManager::EditorUI( Registry& _reg, EntityType _e )
	{
		for (auto& [id, info] : s_mapComponents)
		{
			if (info.funcCanEdit())
			{
				if (HasComponent( _reg, _e, id ))
				{
					bool bKeepComp = true;
					if (ImGui::CollapsingHeader( info.pName, info.funcCanRemove() ? &bKeepComp : nullptr ))
					{
						info.funcEditorUI( _reg, _e );
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

			for (const auto& [key, value] : s_mapComponents)
			{
				if (!HasComponent(_reg, _e, key))
				{
					ImGui::PushID(key);
					if (ImGui::Selectable(value.pName))
						value.funcAdd(_reg, _e);
					ImGui::PopID();
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
			if (HasComponent(_reg, _e, id))
			{
				auto j = info.funcToJson(_reg, _e);
				if (!j.empty())
					_j[JSON_COMPONENTS][info.pName] = j;
			}
		}
	}

	void ComponentManager::AfterLoad(Registry &_reg, EntityType _e)
	{
		for (auto &[id, info] : s_mapComponents)
		{
			if (HasComponent(_reg, _e, id))
			{
				info.funcAfterLoad(_reg, _e);
			}
		}
	}

	void ComponentManager::DuplicateComponent( Registry& _reg, EntityType _entitySrc, EntityType _entityDest )
	{
		// @check
		for (auto& [id, info] : s_mapComponents)
		{
			info.funcDuplicate( _reg, _entitySrc, _entityDest );
		}
	}

	std::unordered_map<ComponentManager::ComponentTypeId, ComponentManager::ComponentInfo> ComponentManager::s_mapComponents;

	bool ComponentManager::HasComponent( const Registry& _reg, EntityType _e, ComponentTypeId _id )
	{
		ComponentTypeId type[] = { _id };
		return _reg.runtime_view( std::cbegin( type ), std::cend( type ) ).contains( _e );
	}

}