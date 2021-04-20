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
#include "Components/EnemySpawner.h"
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

void TDLevel::Update( Plop::TimeStep& _ts )
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

	EnemySpawnerSystem::OnUpdate( _ts, m_ENTTRegistry );
	EnemySystem::OnUpdate( _ts, m_ENTTRegistry );
	TowerSystem::OnUpdate( _ts, m_ENTTRegistry );

	auto& viewEnemy = m_ENTTRegistry.view<EnemyComponent, Plop::TransformComponent, Plop::AABBColliderComponent>();

	float fMaxDistSq = glm::compMax( Plop::Application::Get()->GetWindow().GetViewportSize() ) * 2.f;
	
	m_ENTTRegistry.view<BulletComponent, Plop::TransformComponent, Plop::AABBColliderComponent>().each( [&]( entt::entity entityID, const BulletComponent& bulletComp, Plop::TransformComponent& transform, const Plop::AABBColliderComponent& collider)
	{
		transform.TranslateWorld( bulletComp.vVelocity * _ts.GetGameDeltaTime() );
		
		bool bDestroyBullet = false;

		const glm::vec2& thisPos2D = transform.GetWorldPosition();

		viewEnemy.each( [&]( entt::entity entityID, EnemyComponent& enemyComp, const Plop::TransformComponent& transformEnemy, const Plop::AABBColliderComponent& colliderEnemy ) {
			if (!enemyComp.IsDead() && !bDestroyBullet)
			{
				if (collider.IsColliding( colliderEnemy, transformEnemy.GetWorldPosition() ))
				{
					bDestroyBullet = true;
					Plop::Entity enemy{ entityID, weak_from_this() };
					if (enemy.HasComponent<Plop::ParticleSystemComponent>())
					{
						auto& enemyParticles = enemy.GetComponent<Plop::ParticleSystemComponent>();
						enemyParticles.Spawn( 20 );
					}

					enemyComp.Hit( bulletComp.emitting.GetComponent<TowerComponent>().fDamage );

					return;
				}
			}
		} );
		

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
}