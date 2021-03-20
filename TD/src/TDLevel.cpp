#include "TD_pch.h"
#include "TDLevel.h"

#include <glm/gtx/component_wise.hpp>

#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Renderer/ParticleSystem.h>
#include <Renderer/Texture.h>
#include <ECS/BaseComponents.h>
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

	std::vector<std::tuple<Plop::Entity, EnemyComponent&, Plop::TransformComponent&>> vecEnemies;
	auto& viewEnemy = m_ENTTRegistry.view<EnemyComponent, Plop::TransformComponent>();
	for (auto& [entityID, enemyComp, transform] : viewEnemy.proxy())
	{
		vecEnemies.push_back( { Plop::Entity( entityID, weak_from_this() ), enemyComp, transform } );
	}

	auto& viewTower = m_ENTTRegistry.view<TowerComponent, Plop::TransformComponent>();
	for (auto& [entityID, towerComp, transform] : viewTower.proxy())
	{
		towerComp.Update( _ts );
		if (towerComp.CanFire())
		{
			float fShortestDistanceSq = -1.f;
			int iBestIndex = -1;
			int index = 0;
			for (auto& enemyTuple : vecEnemies)
			{
				float fDistanceSq = transform.Distance2DSquare( std::get<2>( enemyTuple ) );
				if (fDistanceSq < fShortestDistanceSq || iBestIndex == -1)
				{
					fShortestDistanceSq = fDistanceSq;
					iBestIndex = index;
				}

				++index;
			}

			if (iBestIndex != -1)
			{
				towerComp.Fire( vecEnemies[iBestIndex] );
			}
		}
	}


	auto& viewBullet = m_ENTTRegistry.view<BulletComponent, Plop::TransformComponent>();
	float fMaxDistSq = glm::compMax( Plop::Application::Get()->GetWindow().GetViewportSize() ) * 2.f;
	for (auto& [entityID, bulletComp, transform] : viewBullet.proxy())
	{
		transform.TranslateWorld( bulletComp.vVelocity * _ts.GetGameDeltaTime() );
		
		bool bDestroy = false;
		// test if on target (if target is dead, keep going)
		if (bulletComp.target)
		{
			const auto& enemyTransform = bulletComp.target.GetComponent<Plop::TransformComponent>();

			const glm::vec2& thisPos2D = transform.GetWorldPosition();

			if (enemyTransform.Distance2DSquare(transform ) < 0.001f)
			{
				bDestroy = true;
				auto& enemyParticles = bulletComp.target.GetComponent<Plop::ParticleSystemComponent>();
				enemyParticles.Spawn( 20 );

				auto& enemyComp = bulletComp.target.GetComponent<EnemyComponent>();
				enemyComp.Hit( bulletComp.emitting.GetComponent<TowerComponent>().fDamage );
			}
		}
		

		// test if too far
		if (glm::distance2( transform.GetWorldPosition(), bulletComp.emitting.GetComponent<Plop::TransformComponent>().GetWorldPosition() ) > fMaxDistSq)
			bDestroy = true;

		if (bDestroy)
			m_ENTTRegistry.destroy(entityID);
	}

	Plop::LevelBase::Update( _ts );
}

void TDLevel::UpdateInEditor( Plop::TimeStep _ts )
{
	Plop::LevelBase::UpdateInEditor( _ts );

	//m_particlesBullet.Update( _ts );
}