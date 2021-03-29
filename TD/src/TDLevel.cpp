#include "TD_pch.h"
#include "TDLevel.h"

#include <glm/gtx/component_wise.hpp>

#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Renderer/ParticleSystem.h>
#include <Renderer/Texture.h>
#include <ECS/BaseComponents.h>
#include <ECS/PhysicsComponents.h>
#include <ECS/ParticleSpawners.h>

#include "Components/Bullet.h"
#include "Components/Enemy.h"
#include "Components/Tower.h"
#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?


TDLevel::TDLevel()
{

}

TDLevel::~TDLevel()
{

}

void TDLevel::Init()
{
	Plop::LevelBase::Init();
}

void TDLevel::Update( Plop::TimeStep _ts )
{
	PROFILING_FUNCTION();


	if (Plop::Input::IsMouseLeftPressed() && !m_xCurrentCamera.expired())
	{
		Plop::Entity newEnemy = CreateEntity( "Enemy" );
		auto& enemyComp = newEnemy.AddComponent<EnemyComponent>();
		enemyComp.fLife = 1.f;
		auto& transform = newEnemy.GetComponent<Plop::TransformComponent>();
		glm::vec3 vMousePos = m_xCurrentCamera.lock()->GetWorldPosFromViewportPos( Plop::Input::GetCursorViewportPos(), transform.GetWorldPosition().z );
		transform.SetLocalPosition( vMousePos );
		transform.SetLocalScale( VEC3_1 * 0.2f );
		auto& xSprite = newEnemy.AddComponent<Plop::SpriteRendererComponent>().xSprite;
		xSprite->SetTint( COLOR_GREEN );
	}

	auto& viewEnemySpawner = m_ENTTRegistry.view<EnemySpawnerComponent, Plop::TransformComponent>();
	for (auto& [entityID, enemySpawnerComp, transform] : viewEnemySpawner.proxy())
	{
		enemySpawnerComp.Update( _ts.GetGameDeltaTime() );
	}

	auto& viewEnemy = m_ENTTRegistry.view<EnemyComponent, Plop::TransformComponent, Plop::AABBColliderComponent>();

	viewEnemy.each( [&]( entt::entity entityID, EnemyComponent& enemyComp, const Plop::TransformComponent& transformEnemy, const Plop::AABBColliderComponent& colliderEnemy ) {
		if (enemyComp.IsDead())
		{
			Plop::Entity e{ entityID, weak_from_this() };
			e.Destroy();
		}
		else
		{
			enemyComp.Move( _ts.GetGameDeltaTime() );
		}
	} );

	m_ENTTRegistry.view<TowerComponent, Plop::TransformComponent>().each( [&]( TowerComponent& towerComp, Plop::TransformComponent& transform )
	{
		towerComp.Update( _ts );
		if (towerComp.CanFire())
		{
			float fShortestDistanceSq = -1.f;
			entt::entity iBestEnemy = entt::null;
			glm::vec3 vEnemyPos;

			viewEnemy.each( [&]( entt::entity entityID, const EnemyComponent& enemyComp, const Plop::TransformComponent& transformEnemy, const Plop::AABBColliderComponent& colliderEnemy ) {
				float fDistanceSq = transform.Distance2DSquare( transformEnemy );
				if (fDistanceSq < fShortestDistanceSq || iBestEnemy == entt::null)
				{
					fShortestDistanceSq = fDistanceSq;
					iBestEnemy = entityID;
					vEnemyPos = transformEnemy.GetWorldPosition();
				}
			} );

			if (iBestEnemy != entt::null)
			{

				towerComp.Fire( Plop::Entity{ iBestEnemy, weak_from_this() }, vEnemyPos );
			}
		}
	} );


	float fMaxDistSq = glm::compMax( Plop::Application::Get()->GetWindow().GetViewportSize() ) * 2.f;
	
	m_ENTTRegistry.view<BulletComponent, Plop::TransformComponent, Plop::AABBColliderComponent>().each( [&]( entt::entity entityID, const BulletComponent& bulletComp, Plop::TransformComponent& transform, const Plop::AABBColliderComponent& collider)
	{
		transform.TranslateWorld( bulletComp.vVelocity * _ts.GetGameDeltaTime() );
		
		bool bDestroyBullet = false;
		// test if on target (if target is dead, keep going)
		if (bulletComp.target)
		{
			const auto& enemyTransform = bulletComp.target.GetComponent<Plop::TransformComponent>();

			const glm::vec2& thisPos2D = transform.GetWorldPosition();

			viewEnemy.each( [&]( entt::entity entityID, const EnemyComponent& enemyComp, const Plop::TransformComponent& transformEnemy, const Plop::AABBColliderComponent& colliderEnemy ) {
				if (!enemyComp.IsDead())
				{
					if (collider.IsColliding( colliderEnemy, transformEnemy.GetWorldPosition() ))
					{
						bDestroyBullet = true;
						if (bulletComp.target.HasComponent<Plop::ParticleSystemComponent>())
						{
							auto& enemyParticles = bulletComp.target.GetComponent<Plop::ParticleSystemComponent>();
							enemyParticles.Spawn( 20 );
						}

						auto& enemyComp = bulletComp.target.GetComponent<EnemyComponent>();
						enemyComp.Hit( bulletComp.emitting.GetComponent<TowerComponent>().fDamage );

						return;
					}
				}
			} );
		}
		

		// test if too far
		if (glm::distance2( transform.GetWorldPosition(), bulletComp.emitting.GetComponent<Plop::TransformComponent>().GetWorldPosition() ) > fMaxDistSq)
			bDestroyBullet = true;

		if (bDestroyBullet)
		{
			DestroyEntity( Plop::Entity{ entityID, weak_from_this() } );
		}
	});

	Plop::LevelBase::Update( _ts );
}

void TDLevel::UpdateInEditor( Plop::TimeStep _ts )
{
	Plop::LevelBase::UpdateInEditor( _ts );

	//m_particlesBullet.Update( _ts );




	auto& viewAABB = m_ENTTRegistry.view<Plop::TransformComponent, Plop::AABBColliderComponent>();
	for (auto& [entityID, transform, collider] : viewAABB.proxy())
	{
		bool bCollides = false;
		for (auto& [entityID2, transform2, collider2] : viewAABB.proxy())
		{
			if (entityID == entityID2)
				continue;

			glm::vec3 vWorldPos = transform2.GetWorldPosition();
			if (collider.IsColliding( collider2, vWorldPos ))
			{
				bCollides = true;
				break;
			}
		}

		glm::vec3 vWorldPos = transform.GetWorldPosition();
		Plop::EditorGizmo::AABB( collider.vMin + vWorldPos, collider.vMax + vWorldPos, bCollides ? COLOR_RED : COLOR_GREEN );
	}
}