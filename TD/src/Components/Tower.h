#pragma once

#include <imgui_entt_entity_editor.hpp>
#include <ECS/TransformComponent.h>

#include "Components/Enemy.h"


struct TowerComponent
{
	float fDamage = 1.f;
	float fFiringRate = 1.f; // fire per second

	bool CanFire() const;
	
	// internal
	float fFireDelay = 0.f;
};

namespace TowerSystem
{
	void OnUpdate( const Plop::TimeStep& _ts, entt::registry& _registry );
}

namespace MM
{
	template <>	void ComponentEditorWidget<TowerComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<TowerComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<TowerComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}