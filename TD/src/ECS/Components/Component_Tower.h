#pragma once

#include <imgui_entt_entity_editor.hpp>
#include <ECS/Components/Component_Transform.h>

#include "ECS/Components/Component_Enemy.h"


struct Component_Tower
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

#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>	void ComponentEditorWidget<Component_Tower>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Component_Tower>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Component_Tower>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}
#endif