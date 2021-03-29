#pragma once

#include <imgui_entt_entity_editor.hpp>

#include <Math/Curves.h>


struct EnemyComponent
{
	float fLife = 1.f;
	float fMoveSpeed = 0.2f;

	void Hit( float _fDamage );
	void Move( float _fDeltaTime );
	bool IsDead() const;


	Plop::Math::CatmullRomCurvePtr xPathCurve;
	float fPathPosition = 0.f;
};

struct EnemySpawnerComponent
{
	struct Wave
	{
		int nbEnemies = 5;
		float fSpawnDelay = 0.5f;
	};

	EnemySpawnerComponent();


	void Update( float _deltaTime );
	void Spawn();


	Plop::Math::CatmullRomCurvePtr xPathCurve;

	Wave wave;

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
