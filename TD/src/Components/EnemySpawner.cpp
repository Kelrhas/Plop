#include "TD_pch.h"
#include "EnemySpawner.h"

#include <glm/gtc/type_ptr.hpp>

#include <Application.h>
#include <ECS/ECSHelper.h>
#include <ECS/BaseComponents.h>
#include <Assets/SpritesheetLoader.h>
#include <ECS/PhysicsComponents.h>
#include <ECS/TransformComponent.h>
#include <Utils/JsonTypes.h>

#include "Enemy.h"

#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?

EnemySpawnerComponent::EnemySpawnerComponent()
{
	xPathCurve.reset( new Plop::Math::CatmullRomCurve );
}

void EnemySpawnerSystem::OnUpdate( const Plop::TimeStep& _ts, entt::registry& _registry )
{
	auto& viewEnemySpawner = _registry.view<EnemySpawnerComponent>();
	viewEnemySpawner.each([&_ts]( const entt::entity entity, EnemySpawnerComponent& spawner) {

		if (spawner.fTimer >= 0 && spawner.iNbEnemySpawned < spawner.wave.nbEnemies)
		{
			spawner.fTimer -= _ts.GetGameDeltaTime();

			if (spawner.fTimer <= 0)
			{
				++spawner.iNbEnemySpawned;
				spawner.fTimer = spawner.wave.fSpawnDelay;

				// create an Enemy
				// TODO: use a prefab
				Plop::LevelBasePtr xLevel = Plop::LevelBase::GetCurrentLevel().lock();
				auto entityEnemy = xLevel->CreateEntity( "Enemy" );

				Plop::Entity owner{ entity, xLevel };
				entityEnemy.SetParent( owner );

				auto& enemyComp = entityEnemy.AddComponent<EnemyComponent>();
				enemyComp.xPathCurve = spawner.xPathCurve;

				auto& spriteComp = entityEnemy.AddComponent<Plop::SpriteRendererComponent>();
				auto hSpritesheet = Plop::AssetLoader::GetSpritesheet( Plop::Application::Get()->GetRootDirectory() / "assets/textures/tiles.ssdef" );
				if (hSpritesheet)
				{
					spriteComp.xSprite->SetSpritesheet( hSpritesheet, "enemy" );
				}

				Plop::AABBColliderComponent& colliderComp = entityEnemy.AddComponent<Plop::AABBColliderComponent>();
				colliderComp.vMin = glm::vec3( -0.15f, -0.15f, -10.f );
				colliderComp.vMax = glm::vec3( 0.15f, 0.15f, 10.f );

				auto& transformComp = entityEnemy.GetComponent<Plop::TransformComponent>();
				transformComp.SetWorldPosition( owner.GetComponent<Plop::TransformComponent>().GetWorldPosition() + VEC3_Z * (0.1f * (spawner.iNbEnemySpawned + 1)) );

			}
		}
	});
}

namespace MM
{
	template <>
	void ComponentEditorWidget<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<EnemySpawnerComponent>( e );

		Plop::LevelBasePtr xLevel = Plop::LevelBase::GetCurrentLevel().lock();
		Plop::Entity owner = Plop::GetComponentOwner( xLevel, comp );
		const auto& vSpawnerPosition = owner.GetComponent<Plop::TransformComponent>().GetWorldPosition();


		size_t iNbPoint = comp.xPathCurve->vecControlPoints.size();
		if (ImGui::TreeNode( &comp, "Number of points: %llu", iNbPoint ))
		{
			int indexToRemove = -1;
			for (int i = 0; i < iNbPoint; ++i)
			{
				ImGui::PushID( i );

				glm::vec3& vPoint = comp.xPathCurve->vecControlPoints[i];
				ImGui::DragFloat2( "", glm::value_ptr( vPoint ), 0.1f );
				ImGui::SameLine();
				if (ImGui::Button( "-" ))
				{
					indexToRemove = i;
				}


				const glm::vec3 vCurrentPoint = comp.xPathCurve->vecControlPoints[i] + vSpawnerPosition;
				Plop::EditorGizmo::FilledCircle( vCurrentPoint );

				// draw the curve
				if (i > 0 && i < iNbPoint - 2)
				{
					const glm::vec3 vPrevPoint = i == 0 ? vSpawnerPosition : comp.xPathCurve->vecControlPoints[i - 1] + vSpawnerPosition;
					const glm::vec3 vNextPoint = comp.xPathCurve->vecControlPoints[i + 1] + vSpawnerPosition;
					const glm::vec3 vNext2Point = comp.xPathCurve->vecControlPoints[i + 2] + vSpawnerPosition;
					Plop::EditorGizmo::CatmullRom( vPrevPoint, vCurrentPoint, vNextPoint, vNext2Point );
				}

				ImGui::PopID();
			}


			static float fTest = 0.f;
			ImGui::SliderFloat( "Test point", &fTest, 0.f, (float)iNbPoint - 3 );

			glm::vec vTestPos = comp.xPathCurve->Interpolate( fTest ) + vSpawnerPosition;
			Plop::EditorGizmo::FilledCircle( vTestPos, COLOR_RED );

			glm::vec vTangent = comp.xPathCurve->GetTangent( fTest );
			Plop::EditorGizmo::Line( vTestPos - vTangent, vTestPos + vTangent, COLOR_GREEN );

			if (indexToRemove != -1)
			{
				comp.xPathCurve->vecControlPoints.erase( comp.xPathCurve->vecControlPoints.begin() + indexToRemove );
			}

			if (ImGui::Button( "Add point" ))
			{
				comp.xPathCurve->vecControlPoints.push_back( VEC3_0 );
			}

			ImGui::TreePop();
		}

		ImGui::DragInt( "Nb enemies", &comp.wave.nbEnemies, 0.1f, 1 );
		ImGui::DragFloat( "Spawn delay", &comp.wave.fSpawnDelay, 0.1f, 0.01f );
	}

	template <>
	json ComponentToJson<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<EnemySpawnerComponent>( e );
		json j;
		j["Points"] = comp.xPathCurve->vecControlPoints;
		j["NbEnemies"] = comp.wave.nbEnemies;
		j["Delay"] = comp.wave.fSpawnDelay;
		return j;
	}

	template<>
	void ComponentFromJson<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<EnemySpawnerComponent>( e );
		if (_j.contains( "Points" ))
			_j["Points"].get_to( comp.xPathCurve->vecControlPoints );
		if (_j.contains( "NbEnemies" ))
			comp.wave.nbEnemies = _j["NbEnemies"];
		if (_j.contains( "Delay" ))
			comp.wave.fSpawnDelay = _j["Delay"];
	}
}
