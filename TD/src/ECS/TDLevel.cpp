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


static constexpr float TIME_BETWEEN_WAVES = 5.f;

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
			case GameEventType::WaveFinished:
			{
				m_eState	 = LevelState::WAITING_WAVE;
				m_fTimerWave = TIME_BETWEEN_WAVES;
				break;
			}
			case GameEventType::GameOver:
			{
				m_eState = LevelState::GAME_OVER;
				break;
			}
		}
	}

	if (_pEvent->IsHandled())
		return true;

	return LevelBase::OnEvent(_pEvent);
}

void TDLevel::OnStart()
{
	LevelBase::OnStart();

	m_playerStats = PlayerLevelStats(); // reset stats
	m_eState	  = LevelState::INIT;

	HexgridSystem::OnStart(m_ENTTRegistry);
	PlayerBaseSystem::OnStart(m_ENTTRegistry);
}

void TDLevel::Update(Plop::TimeStep &_ts)
{
	PROFILING_FUNCTION();

	switch (m_eState)
	{
		case LevelState::INIT:
		{
			TowerSystem::OnUpdate(_ts, m_ENTTRegistry);
			HexgridSystem::OnUpdate(_ts, m_ENTTRegistry);
			break;
		}
		case LevelState::PLAYING:
		{
			UpdatePlaying(_ts);
			break;
		}
		case LevelState::WAITING_WAVE:
		{
			UpdateWaiting(_ts);
			break;
		}
	}

	if(ImGui::Begin("Level"))
	{
		ImGui::Text("Money %d", m_playerStats.iMoney);

		switch (m_eState)
		{
			case LevelState::INIT:
			{
				ImGui::Separator();
				ImGui::Text("Place your turrets and click here to start the waves");
				if (ImGui::Button("Start"))
				{
					m_eState = LevelState::PLAYING;
					EnemySpawnerSystem::TriggerNextWave(m_ENTTRegistry);
				}
				break;
			}
			case LevelState::PLAYING:
			{
				ImGui::Separator();
				ImGui::Text("Enemy killed %d", m_playerStats.iNbKill);
				break;
			}
			case LevelState::WAITING_WAVE:
			{
				ImGui::Separator();
				ImGui::Text("Enemy killed %d", m_playerStats.iNbKill);
				if (ImGui::Button(fmt::format("Next wave in {:.3f}###NextWave", m_fTimerWave).c_str()))
				{
					m_eState = LevelState::PLAYING;
					EnemySpawnerSystem::TriggerNextWave(m_ENTTRegistry);
					m_fTimerWave = 0.f;
				}
				break;
			}
			case LevelState::GAME_OVER:
			{
				ImGui::Separator();
				ImGui::Text("GAME OVER");
				break;
			}
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

void TDLevel::UpdatePlaying(Plop::TimeStep &_ts)
{
	PROFILING_FUNCTION();
	PlayerBaseSystem::OnUpdate(_ts, m_ENTTRegistry);
	EnemySpawnerSystem::OnUpdate(_ts, m_ENTTRegistry);
	EnemySystem::OnUpdate(_ts, m_ENTTRegistry);
	TowerSystem::OnUpdate(_ts, m_ENTTRegistry);
	HexgridSystem::OnUpdate(_ts, m_ENTTRegistry);
	BulletSystem::OnUpdate(_ts, m_ENTTRegistry);
}

void TDLevel::UpdateWaiting(Plop::TimeStep &_ts)
{
	PlayerBaseSystem::OnUpdate(_ts, m_ENTTRegistry);
	EnemySpawnerSystem::OnUpdate(_ts, m_ENTTRegistry);
	EnemySystem::OnUpdate(_ts, m_ENTTRegistry);
	TowerSystem::OnUpdate(_ts, m_ENTTRegistry);
	HexgridSystem::OnUpdate(_ts, m_ENTTRegistry);
	BulletSystem::OnUpdate(_ts, m_ENTTRegistry);

	m_fTimerWave -= _ts.GetGameDeltaTime();
	if (m_fTimerWave <= 0.f)
	{
		m_eState = LevelState::PLAYING;
		EnemySpawnerSystem::TriggerNextWave(m_ENTTRegistry);
	}
}
