#include "TD_pch.h"
#include "TDLevel.h"

#include <glm/gtx/component_wise.hpp>

#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>
#include <ECS/Components/Component_ParticleSystem.h>
#include <ECS/Components/Component_Physics.h>
#include <ECS/Components/Component_SpriteRenderer.h>
#include <ECS/ParticleSpawners.h>

#include "Components/Component_Bullet.h"
#include "Components/Component_Enemy.h"
#include "Components/Component_EnemySpawner.h"
#include "Components/Component_Tower.h"
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

	m_grid.Init();
}

void TDLevel::Update( Plop::TimeStep& _ts ) 

{
	PROFILING_FUNCTION();


	if (Plop::Input::IsMouseLeftPressed() && !m_xCurrentCamera.expired())
	{
		glm::vec3 vMousePos = m_xCurrentCamera.lock()->GetWorldPosFromViewportPos( Plop::Input::GetCursorScreenPos(), 0.f );

		
		Plop::Entity newEnemy = CreateEntity( "Enemy" );
		auto& enemyComp = newEnemy.AddComponent<Component_Enemy>();
		enemyComp.fLife = 1.f;
		auto& transform = newEnemy.GetComponent<Plop::Component_Transform>();
		transform.SetLocalPosition( vMousePos );
		transform.SetLocalScale( VEC3_1 * 0.2f );
		auto& xSprite = newEnemy.AddComponent<Plop::Component_SpriteRenderer>().xSprite;
		xSprite->SetTint( COLOR_GREEN );
	}

	EnemySpawnerSystem::OnUpdate( _ts, m_ENTTRegistry );
	EnemySystem::OnUpdate( _ts, m_ENTTRegistry );
	TowerSystem::OnUpdate( _ts, m_ENTTRegistry );

	auto& viewEnemy = m_ENTTRegistry.view<Component_Enemy, Plop::Component_Transform, Plop::Component_AABBCollider>();

	float fMaxDistSq = glm::compMax( Plop::Application::Get()->GetWindow().GetViewportSize() ) * 2.f;
	
	m_ENTTRegistry.view<Component_Bullet, Plop::Component_Transform, Plop::Component_AABBCollider>().each( [&]( entt::entity entityID, const Component_Bullet& bulletComp, Plop::Component_Transform& transform, const Plop::Component_AABBCollider& collider)
	{
		transform.TranslateWorld( bulletComp.vVelocity * _ts.GetGameDeltaTime() );
		
		bool bDestroyBullet = false;

		const glm::vec2& thisPos2D = transform.GetWorldPosition();

		viewEnemy.each( [&]( entt::entity entityID, Component_Enemy& enemyComp, const Plop::Component_Transform& transformEnemy, const Plop::Component_AABBCollider& colliderEnemy ) {
			if (!enemyComp.IsDead() && !bDestroyBullet)
			{
				if (collider.IsColliding( colliderEnemy, transformEnemy.GetWorldPosition() ))
				{
					bDestroyBullet = true;
					Plop::Entity enemy{ entityID, Plop::Application::GetCurrentLevel()};
					if (enemy.HasComponent<Plop::Component_ParticleSystem>())
					{
						auto& enemyParticles = enemy.GetComponent<Plop::Component_ParticleSystem>();
						enemyParticles.Spawn( 20 );
					}

					enemyComp.Hit( bulletComp.emitting.GetComponent<Component_Tower>().fDamage );

					return;
				}
			}
		} );
		

		// test if too far
		if (glm::distance2( transform.GetWorldPosition(), bulletComp.emitting.GetComponent<Plop::Component_Transform>().GetWorldPosition() ) > fMaxDistSq)
			bDestroyBullet = true;

		if (bDestroyBullet)
		{
			DestroyEntity( Plop::Entity{ entityID, Plop::Application::GetCurrentLevel()} );
		}
	});

	Plop::LevelBase::Update( _ts );
}

void TDLevel::UpdateInEditor( Plop::TimeStep _ts )
{
	Plop::LevelBase::UpdateInEditor( _ts );

	//m_particlesBullet.Update( _ts );
}