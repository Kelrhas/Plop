#include "TD_pch.h"
#include "PlayerBase.h"

namespace MM
{
	template <>
	void ComponentEditorWidget<PlayerBaseComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<PlayerBaseComponent>( e );
		ImGui::DragFloat( "Life", &comp.fLife, 0.1f, 1.f, FLT_MAX );
	}

	template <>
	json ComponentToJson<PlayerBaseComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<PlayerBaseComponent>( e );
		json j;
		j["Life"] = comp.fLife;
		return j;
	}

	template<>
	void ComponentFromJson<PlayerBaseComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<PlayerBaseComponent>( e );
		if (_j.contains( "Life" ))
			comp.fLife = _j["Life"];
	}
}