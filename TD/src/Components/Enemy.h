#pragma once

#include <imgui_entt_entity_editor.hpp>


struct EnemyComponent
{
	float fLife = 1.f;

	void Hit( float _fDamage );
};

struct EnemySpawnerComponent
{
	struct Wave
	{
		int nbEnemies = 5;
		float fSpawnDelay = 0.5f;
	};


	std::vector<glm::vec3> vecCurvePoints;

	Wave wave;

	void Update( float _deltaTime );
	void Spawn();

private:
	float fTimer = 0;
	int iNbEnemySpawned = 0;
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
