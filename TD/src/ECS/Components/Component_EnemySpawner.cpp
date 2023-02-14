#include "TD_pch.h"

#include "Component_EnemySpawner.h"

#include "Component_Enemy.h"
#include "TDEvents.h"

#include <Application.h>
#include <Assets/SpritesheetLoader.h>
#include <ECS/Components/Component_Physics.h>
#include <ECS/Components/Component_SpriteRenderer.h>
#include <ECS/Components/Component_Transform.h>
#include <ECS/ECSHelper.h>
#include <ECS/PrefabManager.h>
#include <Events/EventDispatcher.h>
#include <Events/GameEvent.h>
#include <Utils/JsonTypes.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_custom.h>

#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122 ?

static constexpr const char *JSON_PATH_CONTROL_POINTS = "ControlPoints";
static constexpr const char *JSON_WAVES				  = "Waves";
static constexpr const char *JSON_WAVE_ENEMIES_COUNT  = "EnemiesCount";
static constexpr const char *JSON_WAVE_ENEMIES_DELAY  = "Delay";
static constexpr const char *JSON_WAVE_ENEMIES_PREFAB = "EnemyPrefab";

template<>
struct nlohmann::adl_serializer<Component_EnemySpawner::Wave>
{
	static void to_json(json &_j, const Component_EnemySpawner::Wave &_w)
	{
		_j[JSON_WAVE_ENEMIES_COUNT]	 = _w.nbEnemies;
		_j[JSON_WAVE_ENEMIES_DELAY]	 = _w.fSpawnDelay;
		_j[JSON_WAVE_ENEMIES_PREFAB] = _w.hEnemy;
	}

	static void from_json(const json &_j, Component_EnemySpawner::Wave &_w)
	{
		if (_j.contains(JSON_WAVE_ENEMIES_COUNT))
			_w.nbEnemies = _j[JSON_WAVE_ENEMIES_COUNT];
		if (_j.contains(JSON_WAVE_ENEMIES_DELAY))
			_w.fSpawnDelay = _j[JSON_WAVE_ENEMIES_DELAY];
		if (_j.contains(JSON_WAVE_ENEMIES_PREFAB))
			_w.hEnemy = _j[JSON_WAVE_ENEMIES_PREFAB];
	}
};

Component_EnemySpawner::Component_EnemySpawner()
{
	xPathCurve.reset(new Plop::Math::CatmullRomCurve);
}

void Component_EnemySpawner::EditorUI()
{
	Plop::LevelBasePtr xLevel			= Plop::Application::GetCurrentLevel().lock();
	Plop::Entity	   owner			= Plop::GetComponentOwner(xLevel->GetEntityRegistry(), *this);
	const auto		  &vSpawnerPosition = owner.GetComponent<Plop::Component_Transform>().GetWorldPosition();


	size_t iNbPoint = xPathCurve->vecControlPoints.size();
	if (ImGui::TreeNode(this, "Show %llu control points", iNbPoint))
	{
		int indexToRemove = -1;
		for (int i = 0; i < iNbPoint; ++i)
		{
			ImGui::PushID(i);

			glm::vec3 &vPoint = xPathCurve->vecControlPoints[i];
			ImGui::DragFloat2("", glm::value_ptr(vPoint), 0.1f);
			ImGui::SameLine();
			if (ImGui::Button("-"))
			{
				indexToRemove = i;
			}


			const glm::vec3 vCurrentPoint = xPathCurve->vecControlPoints[i] + vSpawnerPosition;
			Plop::EditorGizmo::Point(vCurrentPoint);

			// draw the curve
			if (i > 0 && i < iNbPoint - 2)
			{
				const glm::vec3 vPrevPoint	= i == 0 ? vSpawnerPosition : xPathCurve->vecControlPoints[i - 1] + vSpawnerPosition;
				const glm::vec3 vNextPoint	= xPathCurve->vecControlPoints[i + 1] + vSpawnerPosition;
				const glm::vec3 vNext2Point = xPathCurve->vecControlPoints[i + 2] + vSpawnerPosition;
				Plop::EditorGizmo::CatmullRom(vPrevPoint, vCurrentPoint, vNextPoint, vNext2Point);
			}

			ImGui::PopID();
		}


		if (iNbPoint > 3)
		{
			static float fTest = 0.f;
			ImGui::SliderFloat("Test point", &fTest, 0.f, (float)iNbPoint - 3);

			glm::vec vTestPos = xPathCurve->Interpolate(fTest) + vSpawnerPosition;
			Plop::EditorGizmo::Point(vTestPos, COLOR_RED);

			glm::vec vTangent = xPathCurve->GetTangent(fTest);
			Plop::EditorGizmo::Line(vTestPos - vTangent, vTestPos + vTangent, COLOR_GREEN);
		}

		if (indexToRemove != -1)
		{
			xPathCurve->vecControlPoints.erase(xPathCurve->vecControlPoints.begin() + indexToRemove);
		}

		if (ImGui::Button("Add point"))
		{
			xPathCurve->vecControlPoints.push_back(VEC3_0);
		}

		ImGui::TreePop();
	}

	if (ImGui::Button("Add wave"))
	{
		waves.emplace_back();
	}
	int iWave = 0;
	for (auto it = waves.begin(); it != waves.end();)
	{
		bool  bRemoved = false;
		Wave &w = *it;
		if (ImGui::TreeNodeEx(fmt::format("Wave {0}: {1} enemies###Wave{0}", iWave + 1, w.nbEnemies).c_str(), ImGuiTreeNodeFlags_None))
		{
			ImGui::Custom::InputPrefab("Enemy", w.hEnemy);
			ImGui::DragInt("Nb enemies", &w.nbEnemies, 0.1f, 1);
			ImGui::DragFloat("Spawn delay", &w.fSpawnDelay, 0.01f, 0.01f, FLT_MAX);
			if (ImGui::Button("Remove"))
			{
				it = waves.erase(it);
				bRemoved = true;
			}

			ImGui::TreePop();
		}

		if (!bRemoved)
			++it;

		++iWave;
	}
}


json Component_EnemySpawner::ToJson() const
{
	json j;
	j[JSON_PATH_CONTROL_POINTS] = xPathCurve->vecControlPoints;
	j[JSON_WAVES]				= waves;
	return j;
}

void Component_EnemySpawner::FromJson(const json &_j)
{
	if (_j.contains(JSON_PATH_CONTROL_POINTS))
		_j[JSON_PATH_CONTROL_POINTS].get_to(xPathCurve->vecControlPoints);
	if (_j.contains(JSON_WAVES))
		_j[JSON_WAVES].get_to(waves);
}

void Component_EnemySpawner::NextWave()
{
	iWaveIndex = glm::min(iWaveIndex + 1, (int)waves.size()-1);
	iNbEnemySpawned = 0;
}

void EnemySpawnerSystem::TriggerNextWave(entt::registry &_registry)
{
	auto viewEnemySpawner = _registry.view<Component_EnemySpawner>();
	viewEnemySpawner.each([&_registry](const entt::entity entity, Component_EnemySpawner &spawner) { spawner.NextWave(); });
}

void EnemySpawnerSystem::OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry)
{
	PROFILING_FUNCTION();

	auto viewEnemySpawner = _registry.view<Component_EnemySpawner>();
	viewEnemySpawner.each(
	  [&_ts, &_registry](const entt::entity entity, Component_EnemySpawner &spawner)
	  {
		  Component_EnemySpawner::Wave &currentWave = spawner.waves[spawner.iWaveIndex];

		  if (spawner.fTimer >= 0 && spawner.iNbEnemySpawned < currentWave.nbEnemies)
		  {
			  spawner.fTimer -= _ts.GetGameDeltaTime();

			  if (spawner.fTimer <= 0)
			  {
				  ++spawner.iNbEnemySpawned;
				  spawner.fTimer = currentWave.fSpawnDelay;

				  // create an Enemy
				  // TODO: use a prefab
				  Plop::LevelBasePtr xLevel = Plop::Application::GetCurrentLevel().lock();
				  Plop::Entity		 owner { entity, _registry };

				  auto entityEnemy = Plop::PrefabManager::InstantiatePrefab(currentWave.hEnemy, _registry, owner);

				  entityEnemy.SetParent(owner);

				  auto &enemyComp	   = entityEnemy.GetComponent<Component_Enemy>();
				  enemyComp.xPathCurve = spawner.xPathCurve;

				  auto &transformComp = entityEnemy.GetComponent<Plop::Component_Transform>();
				  // stagger the depth to avoid overlapping between enemies
				  transformComp.SetWorldPosition(owner.GetComponent<Plop::Component_Transform>().GetWorldPosition() +
												 VEC3_Z * glm::fract(0.1f * (spawner.iNbEnemySpawned + 1)));

				  if (spawner.iNbEnemySpawned >= currentWave.nbEnemies)
				  {
					  Plop::GameEvent gameEvent((S32)GameEventType::WaveFinished);
					  gameEvent.SetData(spawner.iWaveIndex);
					  Plop::EventDispatcher::SendEvent(gameEvent);
				  }
			  }
		  }
	  });
}
