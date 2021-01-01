#include "TD_pch.h"
#include "TDLevel.h"

#include <glm/gtx/component_wise.hpp>

#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>
#include <ECS/BaseComponents.h>

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
		transform.vPosition += bulletComp.vVelocity * _ts.GetGameDeltaTime();
		
		bool bDestroy = false;
		// test if on target (if target is dead, keep going)
		if (bulletComp.target)
		{
			const glm::vec3& targetPos = bulletComp.target.GetComponent<Plop::TransformComponent>().vPosition;

			if (glm::distance2( targetPos, transform.vPosition ) < 0.001f)
			{
				bDestroy = true;
				auto& enemyComp = bulletComp.target.GetComponent<EnemyComponent>();
				enemyComp.Hit( bulletComp.emitting.GetComponent<TowerComponent>().fDamage );
			}
		}
		

		// test if too far
		if (glm::distance2( transform.vPosition, bulletComp.emitting.GetComponent<Plop::TransformComponent>().vPosition ) > fMaxDistSq)
			bDestroy = true;

		if (bDestroy)
			m_ENTTRegistry.destroy(entityID);
	}

	Plop::LevelBase::Update( _ts );
}