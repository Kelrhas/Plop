#pragma once

#include <imgui_entt_entity_editor.hpp>


struct TowerComponent
{
	float fDamage = 1.f;
	float fFiringRate = 1.f; // per second

	void Update( const Plop::TimeStep& _ts );
	bool CanFire() const;
	void Fire();

private:
	float fFireDelay = 0.f; // 

};

namespace MM
{
	template <>	void ComponentEditorWidget<TowerComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<TowerComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<TowerComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}