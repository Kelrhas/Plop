#include "TD_pch.h"
#include "Tower.h"

void TowerComponent::Update( const Plop::TimeStep& _ts )
{
	if (fFireDelay > 0.f)
	{
		fFireDelay -= _ts.GetGameDeltaTime();
	}
}

bool TowerComponent::CanFire() const
{
	return fFireDelay <= 0.f;
}

void TowerComponent::Fire()
{
	fFireDelay += fFiringRate;
}

namespace MM
{
	template <>
	void ComponentEditorWidget<TowerComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<TowerComponent>( e );
		ImGui::DragFloat( "Damage", &comp.fDamage, 0.1f, 1.f );
		ImGui::DragFloat( "Firing rate", &comp.fFiringRate, 0.1f, 0.01f );
	}

	template <>
	json ComponentToJson<TowerComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<TowerComponent>( e );
		json j;
		j["Damage"] = comp.fDamage;
		j["Firing rate"] = comp.fFiringRate;
		return j;
	}

	template<>
	void ComponentFromJson<TowerComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<TowerComponent>( e );
		if (_j.contains( "Damage" ))
			comp.fDamage = _j["Damage"];
		if (_j.contains( "Firing rate" ))
			comp.fFiringRate = _j["Firing rate"];
	}
}