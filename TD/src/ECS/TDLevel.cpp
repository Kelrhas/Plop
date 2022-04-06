#include "TD_pch.h"
#include "TDLevel.h"

#include <filesystem>
#include <glm/gtx/component_wise.hpp>

#include <Application.h>
#include <Assets/SpritesheetLoader.h>
#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>
#include <ECS/Components/Component_ParticleSystem.h>
#include <ECS/Components/Component_Physics.h>
#include <ECS/Components/Component_SpriteRenderer.h>
#include <ECS/ParticleSpawners.h>
#include <Debug/Log.h>

#include "Components/Component_PlayerBase.h"
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

	LevelGrid::LevelConstraints constraints;
	constraints.m_uLevelHeight = 10;
	constraints.m_uLevelWidth = 10;
	constraints.m_vecStarts.push_back( { 0, 1 } );
	constraints.m_vEnd = { constraints.m_uLevelWidth - 1, constraints.m_uLevelHeight - 2 };

	m_grid.Init( constraints );

	StringPath sSpritesheet = std::filesystem::canonical( Plop::Application::Get()->GetRootDirectory() / "assets/textures/tiles.ssdef" );
	Plop::SpritesheetHandle hSpritesheet = Plop::AssetLoader::GetSpritesheet( sSpritesheet );


	//this level is not the current one if started from editor ...



	m_SpawnerEntity = CreateEntity( "Spawner" );
	m_SpawnerEntity.AddFlag( Plop::EntityFlag::DYNAMIC_GENERATION );
	const glm::vec3 vSpawnerPos(constraints.m_vecStarts.front(), 1.f);
	m_SpawnerEntity.GetComponent<Plop::Component_Transform>().SetWorldPosition(vSpawnerPos);
	m_SpawnerEntity.AddComponent<Component_EnemySpawner>();
	auto& spawnerSpriteComp = m_SpawnerEntity.AddComponent<Plop::Component_SpriteRenderer>();
	spawnerSpriteComp.xSprite->SetSpritesheet( hSpritesheet, "0" );

	const auto &startTile = m_grid.GetTile(0, 1);
	const auto &endTile = m_grid.GetTile(constraints.m_uLevelWidth - 1, constraints.m_uLevelHeight - 2);
	auto &pf = m_grid.GetPathfind(startTile, endTile);
	auto &spawnerComp = m_SpawnerEntity.GetComponent<Component_EnemySpawner>();

	if (pf.bValid)
	{
		const float fDepth = 0.f;
		spawnerComp.xPathCurve->vecControlPoints.clear();
		spawnerComp.xPathCurve->vecControlPoints.push_back(glm::vec3(startTile.vCoord, fDepth) - vSpawnerPos);

		glm::vec3 vLastCoord = glm::vec3(pf.vecPath.front().vCoord, fDepth);
		for (auto &path : pf.vecPath)
		{
			const glm::vec3 vCoord = glm::vec3(path.vCoord, fDepth);
			spawnerComp.xPathCurve->vecControlPoints.push_back((vCoord + vLastCoord) / 2.f - vSpawnerPos);
			vLastCoord = vCoord;
		}

		spawnerComp.xPathCurve->vecControlPoints.push_back(glm::vec3(endTile.vCoord, fDepth) - vSpawnerPos);
	}


	m_BaseEntity = CreateEntity( "Base" );
	m_BaseEntity.AddFlag( Plop::EntityFlag::DYNAMIC_GENERATION );
	m_BaseEntity.GetComponent<Plop::Component_Transform>().SetWorldPosition( glm::vec3( constraints.m_vEnd, 1.f ) );
	auto& baseComp = m_BaseEntity.AddComponent<Component_PlayerBase>();
	baseComp.fLife = 10.f;

	auto& baseSpriteComp = m_BaseEntity.AddComponent<Plop::Component_SpriteRenderer>();
	baseSpriteComp.xSprite->SetSpritesheet( hSpritesheet, "player_base" );
}

void TDLevel::Shutdown()
{
	Plop::LevelBase::Shutdown();

	m_grid.Reset();
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

	auto& editor = Plop::Application::Get()->GetEditor();
	auto& xCamera = editor.GetEditorCamera();

	glm::vec2 vScreenPos = Plop::Input::GetCursorScreenPos();
	vScreenPos = editor.GetViewportPosFromScreenPos( vScreenPos );
	glm::vec3 vMousePos = xCamera->GetWorldPosFromViewportPos( vScreenPos, 0.f );

	if (Plop::Input::IsMouseLeftDown())
	{
		//Plop::Log::Info( "Clicking at {}, {}", vMousePos.x, vMousePos.y );
		if (vMousePos.x >= 0.f && vMousePos.y >= 0.f)
		{
			U32 uXCoord = (U32)round( vMousePos.x );
			U32 uYCoord = (U32)round( vMousePos.y );
			if (uXCoord <= m_grid.uLevelWidth - 1 &&
				uYCoord <= m_grid.uLevelHeight - 1)
			{
				auto& endTile = m_grid.GetTile( uXCoord, uYCoord );

				const auto& startTile = m_grid.GetTile( 1, 1 );
				auto& pf = m_grid.GetPathfind( startTile, endTile );
				auto& spawnerComp = m_SpawnerEntity.GetComponent<Component_EnemySpawner>();
				const glm::vec3 vSpawnerPos = m_SpawnerEntity.GetComponent<Plop::Component_Transform>().GetWorldPosition();

				if (pf.bValid)
				{
					const float fDepth = 0.f;
					spawnerComp.xPathCurve->vecControlPoints.clear();
					spawnerComp.xPathCurve->vecControlPoints.push_back(glm::vec3(startTile.vCoord, fDepth) - vSpawnerPos);

					glm::vec3 vLastCoord = glm::vec3(pf.vecPath.front().vCoord, fDepth);
					for (auto &path : pf.vecPath)
					{
						const glm::vec3 vCoord = glm::vec3(path.vCoord, fDepth);
						spawnerComp.xPathCurve->vecControlPoints.push_back((vCoord + vLastCoord) / 2.f - vSpawnerPos);
						vLastCoord = vCoord;
					}

					spawnerComp.xPathCurve->vecControlPoints.push_back(glm::vec3(endTile.vCoord, fDepth) - vSpawnerPos);
				}
			}
		}
	}
	else
	{
//		Plop::Log::Warn( "Screen coord {},{} -> {}, {}, {}", vScreenPos.x, vScreenPos.y, vMousePos.x, vMousePos.y, vMousePos.z );
	}

	//m_particlesBullet.Update( _ts );
}