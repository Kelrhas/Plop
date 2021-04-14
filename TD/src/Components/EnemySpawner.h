#pragma once

#include <imgui_entt_entity_editor.hpp>

#include <Math/Curves.h>

struct EnemySpawnerComponent
{
	struct Wave
	{
		int nbEnemies = 5;
		float fSpawnDelay = 0.5f;
	};

	EnemySpawnerComponent();


	Plop::Math::CatmullRomCurvePtr xPathCurve;

	Wave wave;
	float fTimer = 0;
	int iNbEnemySpawned = 0;
};


namespace EnemySpawnerSystem
{
	void OnUpdate( const Plop::TimeStep& _ts, entt::registry& _registry );
};

namespace MM
{
	template <>	void ComponentEditorWidget<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}