#include "TD_pch.h"
#include "Enemy.h"



namespace MM
{
	template <>
	void ComponentEditorWidget<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<EnemyComponent>( e );
		ImGui::DragFloat( "Life", &comp.fLife, 0.1f, 1.f );
	}

	template <>
	json ComponentToJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<EnemyComponent>( e );
		json j;
		j["Life"] = comp.fLife;
		return j;
	}

	template<>
	void ComponentFromJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<EnemyComponent>( e );
		if (_j.contains( "Life" ))
			comp.fLife = _j["Life"];
	}
}