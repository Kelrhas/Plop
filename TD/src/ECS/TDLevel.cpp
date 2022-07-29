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
#include <ECS/PrefabManager.h>
#include <Debug/Log.h>

#include "Components/Component_Bullet.h"
#include "Components/Component_Enemy.h"
#include "Components/Component_EnemySpawner.h"
#include "Components/Component_Hexgrid.h"
#include "Components/Component_PlayerBase.h"
#include "Components/Component_Tower.h"
#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122


TDLevel::TDLevel()
{

}

TDLevel::~TDLevel()
{

}

void TDLevel::Init()
{
	Plop::LevelBase::Init();

	Plop::PrefabManager::LoadPrefabLibrary("data/prefabs/Towers.prefablib");
}

void TDLevel::Shutdown()
{
	Plop::LevelBase::Shutdown();
}

void TDLevel::OnStart()
{
	LevelBase::OnStart();

	HexgridSystem::OnStart(m_ENTTRegistry);
}

void TDLevel::Update( Plop::TimeStep& _ts ) 

{
	PROFILING_FUNCTION();

	/*
	if (Plop::Input::IsMouseLeftPressed() && !m_xCurrentCamera.expired())
	{
		// TODO viewport panel
		auto &editor = Plop::Application::Get()->GetEditor();
		const glm::vec2 vViewportPos = editor.GetViewportPosFromWindowPos(Plop::Input::GetCursorWindowPos());
		glm::vec3 vMousePos = m_xCurrentCamera.lock()->GetWorldPosFromViewportPos(vViewportPos, 0.f );

		Hexgrid::CellCoord coord = Hexgrid::Cell::GetCellCoordFrom2D(vMousePos.xy);
		Plop::Log::Info("Coords: {},{},{} -> {},{},{}", vMousePos.x, vMousePos.y, vMousePos.z, coord.x, coord.y, coord.z);
		Plop::Log::Info("	Dist: {}", glm::manhattanDistance(coord, Hexgrid::CellCoord(0, 0, 0)) / 2.f);

		Hexgrid::Cell cellClick;
		if (m_grid.GetCell(coord, &cellClick))
		{
			bool bOverlap = false;
			auto view = m_ENTTRegistry.view<Plop::Component_Transform, Component_Tower>();
			for (auto enttID : view)
			{
				auto &transform = view.get<Plop::Component_Transform>(enttID);

				if (bOverlap)
					return;

				if (glm::length2(transform.GetWorldPosition() - vMousePos) < 1)
				{
					bOverlap = true;
					break;
				}
			}

			if (!bOverlap)
			{
				const Plop::GUID guidTower = 12598841727129875697llu;
				Plop::Entity tower = Plop::PrefabManager::InstantiatePrefab(guidTower, m_ENTTRegistry, Plop::Entity());
				tower.GetComponent<Plop::Component_Transform>().SetWorldPosition(glm::vec3(Hexgrid::Cell::Get2DCoordFromCell(coord), 1.f));
			}
		}
	}
	*/

	PlayerBaseSystem::OnUpdate(_ts, m_ENTTRegistry);
	EnemySpawnerSystem::OnUpdate(_ts, m_ENTTRegistry);
	EnemySystem::OnUpdate(_ts, m_ENTTRegistry);
	TowerSystem::OnUpdate(_ts, m_ENTTRegistry);
	HexgridSystem::OnUpdate(_ts, m_ENTTRegistry);

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
#ifdef USE_ENTITY_HANDLE
					Plop::Entity enemy(entityID, m_ENTTRegistry);
#else
					Plop::Entity enemy(entityID, Plop::Application::GetCurrentLevel());
#endif
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

#ifdef USE_ENTITY_HANDLE
			DestroyEntity(Plop::Entity(entityID, m_ENTTRegistry));
#else
			DestroyEntity(Plop::Entity(entityID, Plop::Application::GetCurrentLevel()));
#endif
		}
	});

	Plop::LevelBase::Update( _ts );
}

void TDLevel::OnStop()
{
	LevelBase::OnStop();

	HexgridSystem::OnStop(m_ENTTRegistry);
}

void TDLevel::UpdateInEditor( Plop::TimeStep _ts )
{
	Plop::LevelBase::UpdateInEditor( _ts );

	HexgridSystem::OnUpdateEditor(_ts, m_ENTTRegistry);
}