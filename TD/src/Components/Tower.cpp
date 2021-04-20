#include "TD_pch.h"
#include "Tower.h"

#include <entt/meta/resolve.hpp>

#include <ECS/LevelBase.h>
#include <ECS/ECSHelper.h>
#include <ECS/BaseComponents.h>
#include <ECS/PhysicsComponents.h>
#include <ECS/AudioEmitter.h>
#include <Assets/TextureLoader.h>

#include "Components/Bullet.h"

#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?

bool TowerComponent::CanFire() const
{
	return fFireDelay <= 0.f;
}

namespace TowerSystem
{
	auto ShootAt = []( entt::entity entityTower, TowerComponent& tower, Plop::TransformComponent& transformTower, const glm::vec3& _vEnemyPosition ) {

		tower.fFireDelay += 1.f / tower.fFiringRate;

		Plop::LevelBasePtr xLevel = Plop::LevelBase::GetCurrentLevel().lock();
		glm::vec3 vTowerPos = transformTower.GetWorldPosition();

		const glm::vec2 vEnemyDir2D = glm::normalize( _vEnemyPosition.xy - vTowerPos.xy );
		float fAngle = glm::acos( glm::dot( glm::vec2( 1.f, 0.f ), vEnemyDir2D ) ) - glm::half_pi<float>();
		if (vTowerPos.y > _vEnemyPosition.y)
			fAngle = glm::pi<float>() - fAngle;
		glm::vec3 vTowerRotation = glm::eulerAngles( transformTower.GetLocalRotation() );
		vTowerRotation.z = fAngle;
		transformTower.SetLocalRotation( glm::quat( vTowerRotation ) );

		// Instantiate a Bullet
		{
			Plop::Entity bullet = xLevel->CreateEntity( "Bullet" );

			Plop::SpriteRendererComponent& spriteComp = bullet.AddComponent<Plop::SpriteRendererComponent>();
			spriteComp.xSprite->SetTint( COLOR_RED );

			BulletComponent& bulletComp = bullet.AddComponent<BulletComponent>();
			bulletComp.emitting = { entityTower, xLevel };
			bulletComp.vVelocity = glm::vec3( bulletComp.fSpeed * vEnemyDir2D, 0.f );

			Plop::AABBColliderComponent& colliderComp = bullet.AddComponent<Plop::AABBColliderComponent>();
			colliderComp.vMin = glm::vec3( -0.1f, -0.1f, -10.f );
			colliderComp.vMax = glm::vec3( 0.1f, 0.1f, 10.f );


			Plop::TransformComponent& transform = bullet.GetComponent<Plop::TransformComponent>();
			transform.SetLocalPosition( glm::vec3( vTowerPos.xy, vTowerPos.z - 0.1f ) );
			transform.SetLocalRotation( glm::quat( glm::vec3( 0.f, 0.f, fAngle ) ) );
			transform.SetLocalScale( glm::vec3( 0.2f, 0.2f, 1.f ) );
		}

		// play sound
		Plop::Entity towerEntity{ entityTower, Plop::LevelBase::GetCurrentLevel() };
		auto& audioComp = towerEntity.GetComponent<Plop::AudioEmitterComponent>();
		audioComp.PlaySound();

	};


	void OnUpdate( const Plop::TimeStep& _ts, entt::registry& _registry )
	{
		auto& viewEnemy = _registry.view<EnemyComponent, Plop::TransformComponent>();

		_registry.view<TowerComponent, Plop::TransformComponent>().each( [&]( entt::entity entityTower, TowerComponent& tower, Plop::TransformComponent& transform ) 	{

			if (tower.fFireDelay > 0.f)
			{
				tower.fFireDelay -= _ts.GetGameDeltaTime();
			}

			if (tower.CanFire())
			{
				float fShortestDistanceSq = -1.f;
				entt::entity iBestEnemy = entt::null;
				glm::vec3 vEnemyPos;

				viewEnemy.each( [&]( entt::entity entityID, const EnemyComponent&, const Plop::TransformComponent& transformEnemy ) {
					float fDistanceSq = transform.Distance2DSquare( transformEnemy );
					if (fDistanceSq < fShortestDistanceSq || iBestEnemy == entt::null)
					{
						fShortestDistanceSq = fDistanceSq;
						iBestEnemy = entityID;
						vEnemyPos = transformEnemy.GetWorldPosition();
					}
				} );

				if (iBestEnemy != entt::null)
				{
					ShootAt( entityTower, tower, transform, vEnemyPos );
					//tower.Fire( Plop::Entity{ iBestEnemy, Plop::LevelBase::GetCurrentLevel() }, vEnemyPos );
				}
			}
		} );
	}
}

namespace MM
{
	template <>
	void ComponentEditorWidget<TowerComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<TowerComponent>( e );
		ImGui::DragFloat( "Damage", &comp.fDamage, 0.1f, 1.f );
		ImGui::DragFloat( "Firing rate", &comp.fFiringRate, 0.005f, 0.01f, FLT_MAX );
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text( "Fire per seconds, which corresponds to a delay of %.3fs between firing", 1.f / comp.fFiringRate );
			ImGui::EndTooltip();
		}
	}

	template <>
	json ComponentToJson<TowerComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<TowerComponent>( e );
		json j;
		j["Damage"] = comp.fDamage;
		j["Firing rate"] = comp.fFiringRate;
		return j;
	}

	template<>
	void ComponentFromJson<TowerComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<TowerComponent>( e );
		if (_j.contains( "Damage" ))
			comp.fDamage = _j["Damage"];
		if (_j.contains( "Firing rate" ))
			comp.fFiringRate = _j["Firing rate"];
	}
}