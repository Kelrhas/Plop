#include "TD_pch.h"
#include "Component_PlayerBase.h"


void Component_PlayerBase::EditorUI()
{
	ImGui::DragFloat( "Life", &fLife, 0.1f, 1.f, FLT_MAX );
}

json Component_PlayerBase::ToJson() const
{
	json j;
	j["Life"] = fLife;
	return j;
}
void Component_PlayerBase::FromJson( const json& _j )
{
	if (_j.contains( "Life" ))
		fLife = _j["Life"];
}


#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>
	void ComponentEditorWidget<Component_PlayerBase>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Component_PlayerBase>( e );
		comp.EditorUI();
	}

	template <>
	json ComponentToJson<Component_PlayerBase>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Component_PlayerBase>( e );
		return comp.ToJson();
	}

	template<>
	void ComponentFromJson<Component_PlayerBase>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Component_PlayerBase>( e );
		comp.FromJson( _j );
	}
}
#endif