#include "TD_pch.h"

#include "TDLevel.h"

#include "Components/Component_Bullet.h"
#include "Components/Component_Enemy.h"
#include "Components/Component_EnemySpawner.h"
#include "Components/Component_Hexgrid.h"
#include "Components/Component_PlayerBase.h"
#include "Components/Component_Tower.h"
#include "TDEvents.h"

#include <Application.h>
#include <Assets/SpritesheetLoader.h>
#include <Debug/Log.h>
#include <Events/GameEvent.h>
#include <ECS/Components/Component_ParticleSystem.h>
#include <ECS/Components/Component_Physics.h>
#include <ECS/Components/Component_SpriteRenderer.h>
#include <ECS/ParticleSpawners.h>
#include <ECS/PrefabManager.h>
#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>
#include <filesystem>
#include <glm/gtx/component_wise.hpp>
#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122


TDLevel::TDLevel() {}

TDLevel::~TDLevel() {}

void TDLevel::Init()
{
	Plop::LevelBase::Init();

	Plop::PrefabManager::LoadPrefabLibrary("data/prefabs/Towers.prefablib");
}

void TDLevel::Shutdown()
{
	Plop::LevelBase::Shutdown();
}

bool TDLevel::OnEvent(Plop::Event *_pEvent)
{
	if (_pEvent->GetEventType() == Plop::EventType::GameEvent)
	{
		Plop::GameEvent &gameEvent = *(Plop::GameEvent *)_pEvent;
		switch ((GameEventType)gameEvent.m_iGameEventType)
		{
			case GameEventType::EnemyKilled:
			{
				m_playerStats.iNbKill++;
				break;
			}
			case GameEventType::LifeLost:
			{
				break;
			}
			case GameEventType::GameOver:
			{
				m_bPlaying = false;
				break;
			}
		}
	}

	return false;
}

void TDLevel::OnStart()
{
	LevelBase::OnStart();

	m_playerStats = PlayerLevelStats(); // reset stats
	m_bPlaying	  = true;

	HexgridSystem::OnStart(m_ENTTRegistry);
	PlayerBaseSystem::OnStart(m_ENTTRegistry);
}

void TDLevel::Update(Plop::TimeStep &_ts)
{
	PROFILING_FUNCTION();


	if (m_bPlaying)
	{
		PlayerBaseSystem::OnUpdate(_ts, m_ENTTRegistry);
		EnemySpawnerSystem::OnUpdate(_ts, m_ENTTRegistry);
		EnemySystem::OnUpdate(_ts, m_ENTTRegistry);
		TowerSystem::OnUpdate(_ts, m_ENTTRegistry);
		HexgridSystem::OnUpdate(_ts, m_ENTTRegistry);

		auto viewEnemy = m_ENTTRegistry.view<Component_Enemy, Plop::Component_Transform, Plop::Component_AABBCollider>();

		float fMaxDistSq = glm::compMax(Plop::Application::Get()->GetWindow().GetViewportSize()) * 2.f;

		m_ENTTRegistry.view<Component_Bullet, Plop::Component_Transform, Plop::Component_AABBCollider>().each(
		  [&](entt::entity entityID, const Component_Bullet &bulletComp, Plop::Component_Transform &transform, const Plop::Component_AABBCollider &collider) {
			  transform.TranslateWorld(bulletComp.vVelocity * _ts.GetGameDeltaTime());

			  bool bDestroyBullet = false;

			  const glm::vec2 &thisPos2D = transform.GetWorldPosition();

			  {
				  PROFILING_SCOPE("View Enemy");
				  viewEnemy.each([&](entt::entity						 entityID,
									 Component_Enemy &					 enemyComp,
									 const Plop::Component_Transform &	 transformEnemy,
									 const Plop::Component_AABBCollider &colliderEnemy) {
					  if (!enemyComp.IsDead() && !bDestroyBullet)
					  {
						  if (collider.IsColliding(colliderEnemy, transformEnemy.GetWorldPosition()))
						  {
							  bDestroyBullet = true;
#ifdef USE_ENTITY_HANDLE
							  Plop::Entity enemy(entityID, m_ENTTRegistry);
#else
							  Plop::Entity enemy(entityID, Plop::Application::GetCurrentLevel());
#endif
							  if (enemy.HasComponent<Plop::Component_ParticleSystem>())
							  {
								  auto &enemyParticles = enemy.GetComponent<Plop::Component_ParticleSystem>();
								  enemyParticles.Spawn(20);
							  }

							  enemyComp.Hit(bulletComp.emitting.GetComponent<Component_Tower>().fDamage);

							  return;
						  }
					  }
				  });
			  }


			  // test if too far
			  if (glm::distance2(transform.GetWorldPosition(), bulletComp.emitting.GetComponent<Plop::Component_Transform>().GetWorldPosition()) > fMaxDistSq)
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
	}

	if(ImGui::Begin("Level"))
	{
		ImGui::Text("Money %d", m_playerStats.iMoney);
		ImGui::Text("Enemy killed %d", m_playerStats.iNbKill);

		if (!m_bPlaying)
		{
			ImGui::Separator();
			ImGui::Text("GAME OVER");
		}
	}
	ImGui::End();


	Plop::LevelBase::Update(_ts);
}

void TDLevel::OnStop()
{
	LevelBase::OnStop();

	HexgridSystem::OnStop(m_ENTTRegistry);
	PlayerBaseSystem::OnStop(m_ENTTRegistry);
}

void TDLevel::UpdateInEditor(const Plop::TimeStep &_ts)
{
	Plop::LevelBase::UpdateInEditor(_ts);

	HexgridSystem::OnUpdateEditor(_ts, m_ENTTRegistry);
}
