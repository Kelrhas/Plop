#include "TD_pch.h"

#include "Component_Tower.h"

#include "ECS/Components/Component_Bullet.h"

#include <Assets/TextureLoader.h>
#include <ECS/Components/Component_AudioEmitter.h>
#include <ECS/Components/Component_Physics.h>
#include <ECS/Components/Component_SpriteRenderer.h>
#include <ECS/ECSHelper.h>
#include <ECS/LevelBase.h>
#include <Utils/JsonTypes.h>
#include <entt/meta/resolve.hpp>

#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122 ?


static constexpr const char *JSON_DAMAGE		  = "Damage";
static constexpr const char *JSON_FIRING_RATE	  = "Firing rate";
static constexpr const char *JSON_RANGE			  = "Range";
static constexpr const char *JSON_FIRING_PARTICLE = "FiringParticlePrefab";
static constexpr const char *JSON_PROJECTILE	  = "ProjectilePrefab";


void Component_Tower::EditorUI()
{
	ImGui::DragFloat("Damage", &fDamage, 0.1f, 1.f);
	ImGui::DragFloat("Firing rate", &fFiringRate, 0.005f, 0.01f, FLT_MAX);
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Fire per seconds, which corresponds to a delay of %.3fs between firing", 1.f / fFiringRate);
		ImGui::EndTooltip();
	}
	ImGui::DragFloat("Range", &fRange, 0.005f, 0.01f, FLT_MAX);
	if (ImGui::IsItemHovered() || ImGui::IsItemActive())
	{
		auto &		 reg   = Plop::Application::GetCurrentLevel().lock()->GetEntityRegistry();
		Plop::Entity owner = Plop::GetComponentOwner(reg, *this);
		Plop::EditorGizmo::Circle(owner.GetComponent<Plop::Component_Transform>().GetWorldPosition(), fRange);
	}
	ImGui::Custom::InputPrefab("Firing particles", hFiringParticle);
	ImGui::Custom::InputPrefab("Projectile", hProjectile);
}

json Component_Tower::ToJson() const
{
	json j;
	j[JSON_DAMAGE]			= fDamage;
	j[JSON_FIRING_RATE]		= fFiringRate;
	j[JSON_RANGE]			= fRange;
	j[JSON_FIRING_PARTICLE] = hFiringParticle;
	j[JSON_PROJECTILE]		= hProjectile;
	return j;
}

void Component_Tower::FromJson(const json &_j)
{
	if (_j.contains(JSON_DAMAGE))
		fDamage = _j[JSON_DAMAGE];
	if (_j.contains(JSON_FIRING_RATE))
		fFiringRate = _j[JSON_FIRING_RATE];
	if (_j.contains(JSON_RANGE))
		fRange = _j[JSON_RANGE];
	if (_j.contains(JSON_FIRING_PARTICLE))
		hFiringParticle = _j[JSON_FIRING_PARTICLE];
	if (_j.contains(JSON_PROJECTILE))
		hProjectile = _j[JSON_PROJECTILE];
}

bool Component_Tower::CanFire() const
{
	return fFireDelay <= 0.f;
}


namespace TowerSystem
{
	void ShootAt(entt::handle hEntityTower, Component_Tower &towerComp, Plop::Component_Transform &transformTower, const glm::vec3 &_vEnemyPosition) {
		towerComp.fFireDelay += 1.f / towerComp.fFiringRate;

		Plop::LevelBasePtr xLevel	 = Plop::Application::GetCurrentLevel().lock();
		glm::vec3		   vTowerPos = transformTower.GetWorldPosition();

		const glm::vec2 vEnemyDir2D = glm::normalize(_vEnemyPosition.xy - vTowerPos.xy);
		float			fAngle		= glm::acos(glm::dot(glm::vec2(1.f, 0.f), vEnemyDir2D)) - glm::half_pi<float>();
		if (vTowerPos.y > _vEnemyPosition.y)
			fAngle = glm::pi<float>() - fAngle;
		glm::vec3 vTowerRotation = glm::eulerAngles(transformTower.GetLocalRotation());
		vTowerRotation.z		 = fAngle;
		transformTower.SetLocalRotation(glm::quat(vTowerRotation));

		Plop::Entity towerEntity(hEntityTower);

		// Instantiate a Bullet
		glm::vec3 vSpawnPos(vTowerPos.xy, vTowerPos.z - 0.1f);
		towerEntity.ChildVisitor([&vSpawnPos](Plop::Entity child) {
			if (child.GetComponent<Plop::Component_Name>().sName == "Nozzle")
			{
				vSpawnPos = child.GetComponent<Plop::Component_Transform>().GetWorldPosition();
			}
			return VisitorFlow::CONTINUE;
		});

		{
			Plop::Entity bullet;
			if (towerComp.hProjectile)
			{
				bullet = Plop::PrefabManager::InstantiatePrefab(towerComp.hProjectile, hEntityTower.registry(), entt::null);
			}
			else
			{
				bullet = xLevel->CreateEntity("Bullet");

				Plop::Component_SpriteRenderer &spriteComp = bullet.AddComponent<Plop::Component_SpriteRenderer>();
				spriteComp.xSprite->SetTint(COLOR_RED);

				bullet.AddComponent<Component_Bullet>();

				Plop::Component_AABBCollider &colliderComp = bullet.AddComponent<Plop::Component_AABBCollider>();
				colliderComp.vMin						   = glm::vec3(-0.1f, -0.1f, -10.f);
				colliderComp.vMax						   = glm::vec3(0.1f, 0.1f, 10.f);

				Plop::Component_Transform &transform = bullet.GetComponent<Plop::Component_Transform>();
				transform.SetLocalScale(glm::vec3(0.2f, 0.2f, 1.f));
			}

			Component_Bullet &bulletComp = bullet.GetComponent<Component_Bullet>();
			bulletComp.emitting			 = towerEntity;
			bulletComp.vVelocity		 = glm::vec3(bulletComp.fSpeed * vEnemyDir2D, 0.f);

			Plop::Component_Transform &transform = bullet.GetComponent<Plop::Component_Transform>();
			transform.SetLocalPosition(vSpawnPos);
			transform.SetLocalRotation(glm::quat(glm::vec3(0.f, 0.f, fAngle)));
		}

		// play sound
		if (towerEntity.HasComponent<Plop::Component_AudioEmitter>())
		{
			auto &audioComp = towerEntity.GetComponent<Plop::Component_AudioEmitter>();
			audioComp.PlaySound();
		}

		// play FX
		if (towerComp.hFiringParticle)
		{
			Plop::Entity fx = Plop::PrefabManager::InstantiatePrefab(towerComp.hFiringParticle, hEntityTower.registry(), entt::null);
			fx.GetComponent<Plop::Component_Transform>().SetLocalPosition(vSpawnPos);
			fx.GetComponent<Plop::Component_Transform>().SetWorldRotation(glm::rotate(transformTower.GetWorldRotation(), glm::half_pi<float>(), VEC3_Z));
		}
	};


	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry)
	{
		PROFILING_FUNCTION();

		auto viewEnemy = _registry.view<Component_Enemy, Plop::Component_Transform>();

		_registry.view<Component_Tower, Plop::Component_Transform>().each(
		  [&](entt::entity entityTower, Component_Tower &tower, Plop::Component_Transform &transform) {
			  if (tower.fFireDelay > 0.f)
			  {
				  tower.fFireDelay -= _ts.GetGameDeltaTime();
			  }

			  if (tower.CanFire())
			  {
				  const float  fTowerRangeSq	   = tower.fRange * tower.fRange;
				  float		   fShortestDistanceSq = -1.f;
				  entt::entity iBestEnemy		   = entt::null;
				  glm::vec3	   vEnemyPos;

				  viewEnemy.each([&](entt::entity entityID, const Component_Enemy &, const Plop::Component_Transform &transformEnemy) {
					  float fDistanceSq = transform.Distance2DSquare(transformEnemy);
					  if (fDistanceSq < fTowerRangeSq && (fDistanceSq < fShortestDistanceSq || iBestEnemy == entt::null))
					  {
						  fShortestDistanceSq = fDistanceSq;
						  iBestEnemy		  = entityID;
						  vEnemyPos			  = transformEnemy.GetWorldPosition();
					  }
				  });

				  if (iBestEnemy != entt::null)
				  {
					  ShootAt(entt::handle(_registry, entityTower), tower, transform, vEnemyPos);
					  // tower.Fire( Plop::Entity{ iBestEnemy, Plop::Application::GetCurrentLevel() }, vEnemyPos );
				  }
			  }
		  });
	}
} // namespace TowerSystem
