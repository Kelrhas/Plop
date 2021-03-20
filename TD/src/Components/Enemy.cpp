#include "TD_pch.h"
#include "Enemy.h"

#include <glm/gtc/type_ptr.hpp>

#include <ECS/ECSHelper.h>
#include <ECS/BaseComponents.h>

#include <Editor/EditorLayer.h>
#include <Utils/JsonTypes.h>
#include <Assets/TextureLoader.h>

void EnemyComponent::Hit( float _fDamage )
{
	fLife -= _fDamage;
	if (fLife <= 0)
	{
		Plop::LevelBasePtr xLevel = Plop::LevelBase::GetCurrentLevel().lock();
		Plop::Entity owner = Plop::GetComponentOwner( xLevel, *this );
		owner.Destroy();
	}
}


void EnemySpawnerComponent::Update( float _fDeltaTime )
{
	if (fTimer >= 0 && iNbEnemySpawned < wave.nbEnemies)
	{
		fTimer -= _fDeltaTime;

		if (fTimer <= 0)
		{
			Spawn();
			++iNbEnemySpawned;
			fTimer = wave.fSpawnDelay;
		}
	}
}

void EnemySpawnerComponent::Spawn()
{
	Plop::LevelBasePtr xLevel = Plop::LevelBase::GetCurrentLevel().lock();
	auto entity = xLevel->CreateEntity( "Enemy" );

	Plop::Entity owner = Plop::GetComponentOwner( xLevel, *this );
	entity.SetParent( owner );

	auto& enemyComp = entity.AddComponent<EnemyComponent>();
	auto& spriteComp = entity.AddComponent<Plop::SpriteRendererComponent>();
	spriteComp.xSprite->SetTextureHandle( Plop::AssetLoader::GetTexture( "assets\\textures\\tilesheet.png" ) );
	spriteComp.xSprite->SetSpriteIndex( glm::uvec2{ 18,2 }, glm::uvec2{23, 13} );

	//auto& transformComp = entity.GetComponent<Plop::TransformComponent>();
	//transformComp.vPosition = owner.GetComponent<Plop::TransformComponent>().vPosition;
}

namespace MM
{
	template <>
	void ComponentEditorWidget<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<EnemyComponent>( e );
		ImGui::DragFloat( "Life", &comp.fLife, 0.1f, 1.f );
	}

	template <>
	json ComponentToJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<EnemyComponent>( e );
		json j;
		j["Life"] = comp.fLife;
		return j;
	}

	template<>
	void ComponentFromJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<EnemyComponent>( e );
		if (_j.contains( "Life" ))
			comp.fLife = _j["Life"];
	}


	template <>
	void ComponentEditorWidget<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<EnemySpawnerComponent>( e );

		Plop::LevelBasePtr xLevel = Plop::LevelBase::GetCurrentLevel().lock();
		Plop::Entity owner = Plop::GetComponentOwner( xLevel, comp );
		const auto& vSpawnerPosition = owner.GetComponent<Plop::TransformComponent>().GetWorldPosition();


		size_t iNbPoint = comp.vecCurvePoints.size();
		if (ImGui::TreeNode( &comp, "Number of points: %llu", iNbPoint ))
		{
			int indexToRemove = -1;
			for(int i=0; i < iNbPoint; ++i)
			{
				ImGui::PushID( i );

				glm::vec3& vPoint = comp.vecCurvePoints[i];
				ImGui::DragFloat2( "", glm::value_ptr( vPoint ), 0.1f );
				ImGui::SameLine();
				if (ImGui::Button( "-" ))
				{
					indexToRemove = i;
				}


				const glm::vec3 vCurrentPoint = comp.vecCurvePoints[i] + vSpawnerPosition;
				Plop::EditorGizmo::FilledCircle( vCurrentPoint );

				// draw the curve
				if (i < iNbPoint - 2)
				{
					if (i == 0)
						Plop::EditorGizmo::Line( vSpawnerPosition, vCurrentPoint );

					const glm::vec3 vPrevPoint = i == 0 ? vSpawnerPosition : comp.vecCurvePoints[i - 1] + vSpawnerPosition;
					const glm::vec3 vNextPoint = comp.vecCurvePoints[i + 1] + vSpawnerPosition;
					const glm::vec3 vNext2Point = comp.vecCurvePoints[i + 2] + vSpawnerPosition;
					Plop::EditorGizmo::CatmullRom( vPrevPoint, vCurrentPoint, vNextPoint, vNext2Point );

					if( i == iNbPoint -3)
						Plop::EditorGizmo::Line( vNextPoint, vNext2Point );
				}

				ImGui::PopID();
			}

			if (indexToRemove != -1)
			{
				comp.vecCurvePoints.erase( comp.vecCurvePoints.begin() + indexToRemove );
			}

			if (ImGui::Button( "Add point" ))
			{
				comp.vecCurvePoints.push_back( VEC3_0 );
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
		j["Points"] = comp.vecCurvePoints;
		j["NbEnemies"] = comp.wave.nbEnemies;
		j["Delay"] = comp.wave.fSpawnDelay;
		return j;
	}

	template<>
	void ComponentFromJson<EnemySpawnerComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<EnemySpawnerComponent>( e );
		if (_j.contains( "Points" ))
			_j["Points"].get_to( comp.vecCurvePoints );
		if (_j.contains( "NbEnemies" ))
			comp.wave.nbEnemies = _j["NbEnemies"];
		if (_j.contains( "Delay" ))
			comp.wave.fSpawnDelay = _j["Delay"];
	}
}
