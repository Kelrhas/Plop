#include "TD_pch.h"

#include "Component_EnemySpawner.h"

#include "Component_Enemy.h"

#include <Application.h>
#include <Assets/SpritesheetLoader.h>
#include <ECS/Components/Component_Physics.h>
#include <ECS/Components/Component_SpriteRenderer.h>
#include <ECS/Components/Component_Transform.h>
#include <ECS/ECSHelper.h>
#include <ECS/PrefabManager.h>
#include <Utils/JsonTypes.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui_custom.h>

#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122 ?

Component_EnemySpawner::Component_EnemySpawner()
{
	xPathCurve.reset(new Plop::Math::CatmullRomCurve);
}

void Component_EnemySpawner::EditorUI()
{
	Plop::LevelBasePtr xLevel = Plop::Application::GetCurrentLevel().lock();
#ifdef USE_ENTITY_HANDLE
	Plop::Entity owner = Plop::GetComponentOwner(xLevel->GetEntityRegistry(), *this);
#else
	Plop::Entity owner = Plop::GetComponentOwner(xLevel, *this);
#endif
	const auto &vSpawnerPosition = owner.GetComponent<Plop::Component_Transform>().GetWorldPosition();

	ImGui::Custom::InputPrefab("Enemy", wave.hEnemy);

	size_t iNbPoint = xPathCurve->vecControlPoints.size();
	if (ImGui::TreeNode(this, "Number of points: %llu", iNbPoint))
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

	ImGui::DragInt("Nb enemies", &wave.nbEnemies, 0.1f, 1);
	ImGui::DragFloat("Spawn delay", &wave.fSpawnDelay, 0.01f, 0.01f, FLT_MAX);
}

json Component_EnemySpawner::ToJson() const
{
	json j;
	j["Points"]	   = xPathCurve->vecControlPoints;
	j["NbEnemies"] = wave.nbEnemies;
	j["Delay"]	   = wave.fSpawnDelay;
	j["Prefab"]	   = wave.hEnemy;
	return j;
}

void Component_EnemySpawner::FromJson(const json &_j)
{
	if (_j.contains("Points"))
		_j["Points"].get_to(xPathCurve->vecControlPoints);
	if (_j.contains("NbEnemies"))
		wave.nbEnemies = _j["NbEnemies"];
	if (_j.contains("Delay"))
		wave.fSpawnDelay = _j["Delay"];
	if (_j.contains("Prefab"))
		wave.hEnemy = _j["Prefab"];
}

void EnemySpawnerSystem::OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry)
{
	auto &viewEnemySpawner = _registry.view<Component_EnemySpawner>();
	viewEnemySpawner.each([&_ts, &_registry](const entt::entity entity, Component_EnemySpawner &spawner) {
		if (spawner.fTimer >= 0 && spawner.iNbEnemySpawned < spawner.wave.nbEnemies)
		{
			spawner.fTimer -= _ts.GetGameDeltaTime();

			if (spawner.fTimer <= 0)
			{
				++spawner.iNbEnemySpawned;
				spawner.fTimer = spawner.wave.fSpawnDelay;

				// create an Enemy
				// TODO: use a prefab
				Plop::LevelBasePtr xLevel = Plop::Application::GetCurrentLevel().lock();
#ifdef USE_ENTITY_HANDLE
				Plop::Entity owner { entity, _registry };
#else
				Plop::Entity owner { entity, xLevel };
#endif

				auto				 entityEnemy = Plop::PrefabManager::InstantiatePrefab(spawner.wave.hEnemy, _registry, owner);

				entityEnemy.SetParent(owner);

				auto &enemyComp		 = entityEnemy.GetComponent<Component_Enemy>();
				enemyComp.xPathCurve = spawner.xPathCurve;

				auto &transformComp = entityEnemy.GetComponent<Plop::Component_Transform>();
				// stagger the depth to avoid overlapping between enemies
				transformComp.SetWorldPosition(owner.GetComponent<Plop::Component_Transform>().GetWorldPosition() +
											   VEC3_Z * glm::fract(0.1f * (spawner.iNbEnemySpawned + 1))); 
			}
		}
	});
}
