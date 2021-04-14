#include "TD_pch.h"
#include "Enemy.h"

#include <ECS/ECSHelper.h>
#include <ECS/TransformComponent.h>

#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?

void EnemyComponent::Hit( float _fDamage )
{
	fLife -= _fDamage;
}

bool EnemyComponent::IsDead() const
{
	return fLife <= 0.f;
}


namespace EnemySystem
{
	void OnUpdate( const Plop::TimeStep& _ts, entt::registry& _registry )
	{
		const float fDeltaTime = _ts.GetGameDeltaTime();

		auto& viewEnemy = _registry.view<EnemyComponent, Plop::TransformComponent>();
		viewEnemy.each( [fDeltaTime]( const entt::entity entity, EnemyComponent& enemy, Plop::TransformComponent& transform ) {
			
			if (enemy.IsDead())
			{
				Plop::Entity e{ entity, Plop::LevelBase::GetCurrentLevel() };
				e.Destroy();
			}
			else
			{
				if (enemy.xPathCurve)
				{
					//fPathPosition += fMoveSpeed * _fDeltaTime;
					enemy.fPathPosition += enemy.fMoveSpeed / glm::length( enemy.xPathCurve->GetTangent( enemy.fPathPosition ) ) * fDeltaTime;

					glm::vec3 vNewPos = enemy.xPathCurve->Interpolate( enemy.fPathPosition );
					glm::vec3 vTangent = enemy.xPathCurve->GetTangent( enemy.fPathPosition );
					glm::vec3 vTangentDir = glm::normalize( vTangent );
					transform.SetWorldPosition( vNewPos );
					float fAngle = glm::acos( glm::dot( vTangentDir, VEC3_X ) );
					if (vTangentDir.y < 0.f)
						fAngle = -fAngle;
					transform.SetWorldRotation( glm::quat( VEC3_Z * fAngle ) );
				}
			}
			
		} );

	}
};

namespace MM
{
	template <>
	void ComponentEditorWidget<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<EnemyComponent>( e );
		ImGui::DragFloat( "Life", &comp.fLife, 0.1f, 1.f );
	}

	template <>
	json ComponentToJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<EnemyComponent>( e );
		json j;
		j["Life"] = comp.fLife;
		return j;
	}

	template<>
	void ComponentFromJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<EnemyComponent>( e );
		if (_j.contains( "Life" ))
			comp.fLife = _j["Life"];
	}
}
