#include "TD_pch.h"
#include "Component_PlayerBase.h"

namespace MM
{
	template <>
	void ComponentEditorWidget<Component_PlayerBase>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Component_PlayerBase>( e );
		ImGui::DragFloat( "Life", &comp.fLife, 0.1f, 1.f, FLT_MAX );
	}

	template <>
	json ComponentToJson<Component_PlayerBase>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Component_PlayerBase>( e );
		json j;
		j["Life"] = comp.fLife;
		return j;
	}

	template<>
	void ComponentFromJson<Component_PlayerBase>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Component_PlayerBase>( e );
		if (_j.contains( "Life" ))
			comp.fLife = _j["Life"];
	}
}