#pragma once

#include <imgui_entt_entity_editor.hpp>

struct Component_Bullet
{
	float fSpeed = 5.f;


	Component_Bullet() = default;
	Component_Bullet(const Component_Bullet &_other);

	void EditorUI();
	json ToJson() const;
	void FromJson(const json &_j);

	// runtime
	glm::vec3 vVelocity = VEC3_0;
	Plop::Entity emitting;
};

namespace BulletSystem
{
	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry);
}
