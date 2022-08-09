#pragma once

#include <imgui_entt_entity_editor.hpp>

#include <Math/Curves.h>
#include <ECS/PrefabManager.h>

struct Component_EnemySpawner
{
	struct Wave
	{
		int nbEnemies = 5;
		float fSpawnDelay = 0.5f;
		Plop::PrefabHandle hEnemy;
	};

	Component_EnemySpawner();


	Plop::Math::CatmullRomCurvePtr xPathCurve;

	Wave wave;
	float fTimer = 0;
	int iNbEnemySpawned = 0;


	void EditorUI();
	json ToJson() const;
	void FromJson( const json& _j );
};


namespace EnemySpawnerSystem
{
	void OnUpdate( const Plop::TimeStep& _ts, entt::registry& _registry );
};
