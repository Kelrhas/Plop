#pragma once

#include <imgui_entt_entity_editor.hpp>

struct Component_PlayerBase
{
	float fLife = 10.f;

	void OnHit(float _fDamage);

	void EditorUI();
	json ToJson() const;
	void FromJson(const json &_j);
};

namespace PlayerBaseSystem
{
	void OnStart(entt::registry &_registry);
	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry);
	void OnStop(entt::registry &_registry);
}
