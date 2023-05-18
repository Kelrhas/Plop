#pragma once

#include "ECS/Components/Component_Enemy.h"

#include <ECS/Components/Component_Transform.h>
#include <ECS/PrefabManager.h>
#include <imgui_entt_entity_editor.hpp>


struct Component_Tower
{
	float			   fDamage		   = 1.f;
	float			   fImpactRadius   = 0.5;
	float			   fFiringRate	   = 1.f; // fire per second
	float			   fRange		   = 5.f;
	Plop::PrefabHandle hFiringParticle = Plop::GUID::INVALID;
	Plop::PrefabHandle hProjectile	   = Plop::GUID::INVALID;

	void EditorUI();
	json ToJson() const;
	void FromJson(const json &_j);

	bool CanFire() const;

	// internal
	float fFireDelay = 0.f;
};

namespace TowerSystem
{
	void OnStart();
	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry);
	void OnStop();
}
