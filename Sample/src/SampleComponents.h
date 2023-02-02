#pragma once

#include <imgui_entt_entity_editor.hpp>

struct Component_Rotating
{
	float fSpeed = 0.f;
	glm::vec3 vAxis = VEC3_0;

	void EditorUI();
	void FromJson(const json& _j);
	json ToJson();
};
