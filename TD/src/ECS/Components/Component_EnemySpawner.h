#pragma once

#include <ECS/PrefabManager.h>
#include <Math/Curves.h>
#include <imgui_entt_entity_editor.hpp>

struct Component_EnemySpawner
{
	struct Wave
	{
		int				   nbEnemies   = 5;
		float			   fSpawnDelay = 0.5f;
		Plop::PrefabHandle hEnemy;
	};

	Component_EnemySpawner();


	Plop::Math::CatmullRomCurvePtr xPathCurve;

	std::vector<Wave> waves;
	int				  iWaveIndex	  = -1;
	float			  fTimer		  = 0;
	int				  iNbEnemySpawned = 0;


	void EditorUI();
	json ToJson() const;
	void FromJson(const json &_j);

	void NextWave();
};


namespace EnemySpawnerSystem
{
	void TriggerNextWave(entt::registry &_registry);
	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry);
};
