#include "TD_pch.h"
#include "TDLevel.h"


#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>
#include <ECS/BaseComponents.h>

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

	Plop::LevelBase::Update(_ts);

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

			if (index != -1)
			{
				const glm::vec3 vPosition = transform.vPosition;
				const glm::vec3 vEnemyPos = std::get<2>( vecEnemies[iBestIndex] ).vPosition;
				const glm::vec3 vEnemyDir = glm::normalize( vEnemyPos - vPosition );
				transform.vRotation.z = glm::acos( glm::dot( VEC3_RIGHT, vEnemyDir ) ) - glm::half_pi<float>();
				if (transform.vRotation.y > vEnemyPos.y)
					transform.vRotation.z = glm::pi<float>() - transform.vRotation.z;
				towerComp.Fire();
			}
		}
	}
}