#include "TD_pch.h"

#include "Component_Enemy.h"
#include "TDEvents.h"

#include <ECS/Components/Component_Transform.h>
#include <ECS/ECSHelper.h>
#include <Events/EventDispatcher.h>
#include <Events/GameEvent.h>

#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122

void Component_Enemy::Hit(float _fDamage)
{
	fLife -= _fDamage;
	if (fLife <= 0.f)
	{
		Plop::GameEvent gameEvent((S32)GameEventType::EnemyKilled);
		Plop::EventDispatcher::SendEvent(gameEvent);
	}
}

bool Component_Enemy::IsDead() const
{
	return fLife <= 0.f;
}

void Component_Enemy::EditorUI()
{
	ImGui::DragFloat("Life", &fLife, 0.1f, 1.f);
}

json Component_Enemy::ToJson() const
{
	json j;
	j["Life"] = fLife;
	return j;
}

void Component_Enemy::FromJson(const json &_j)
{
	if (_j.contains("Life"))
		fLife = _j["Life"];
}


namespace EnemySystem
{
	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry)
	{
		PROFILING_FUNCTION();

		const float fDeltaTime = _ts.GetGameDeltaTime();

		auto viewEnemy = _registry.view<Component_Enemy, Plop::Component_Transform>();
		viewEnemy.each([fDeltaTime, &_registry](const entt::entity entity, Component_Enemy &enemy, Plop::Component_Transform &transform) {
			if (enemy.IsDead())
			{
#ifdef USE_ENTITY_HANDLE
				Plop::Entity e = { entity, _registry };
#else
				Plop::Entity e = { entity, Plop::Application::GetCurrentLevel() };
#endif
				Plop::Application::GetCurrentLevel().lock()->DestroyEntity(e);
			}
			else
			{
				if (enemy.xPathCurve && enemy.xPathCurve->IsValid())
				{
					// fPathPosition += fMoveSpeed * _fDeltaTime;
					enemy.fPathPosition += enemy.fMoveSpeed / glm::length(enemy.xPathCurve->GetTangent(enemy.fPathPosition)) * fDeltaTime;

					float	  fDepth	  = transform.GetLocalPosition().z;
					glm::vec3 vNewPos	  = enemy.xPathCurve->Interpolate(enemy.fPathPosition);
					vNewPos.z			  = fDepth;
					glm::vec3 vTangent	  = enemy.xPathCurve->GetTangent(enemy.fPathPosition);
					glm::vec3 vTangentDir = glm::normalize(vTangent);
					transform.SetLocalPosition(vNewPos);
					float fAngle = glm::acos(glm::dot(vTangentDir, VEC3_X));
					if (vTangentDir.y < 0.f)
						fAngle = -fAngle;
					transform.SetWorldRotation(glm::quat(VEC3_Z * fAngle));
				}
			}
		});
	}
}; // namespace EnemySystem
