#pragma once

#include <imgui_entt_entity_editor.hpp>


struct EnemyComponent
{
	float fLife = 1.f;

	void Hit( float _fDamage );
};

struct EnemySpawnerComponent
{
	std::vector<glm::vec3> vecCurvePoints;
};

namespace MM
{
	template <>	void ComponentEditorWidget<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );

	template <>	void ComponentEditorWidget<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}
