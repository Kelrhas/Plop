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
}

json Component_Tower::ToJson() const
{
	json j;
	j[JSON_DAMAGE]			= fDamage;
	j[JSON_FIRING_RATE]		= fFiringRate;
	j[JSON_RANGE]			= fRange;
	j[JSON_FIRING_PARTICLE] = hFiringParticle;
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

#ifdef USE_ENTITY_HANDLE
		Plop::Entity towerEntity(hEntityTower);
#else
		Plop::Entity towerEntity { hEntityTower.entity(), xLevel };
#endif

		// Instantiate a Bullet
		glm::vec3 vSpawnPos(vTowerPos.xy, vTowerPos.z - 0.1f);
		towerEntity.ChildVisitor([&vSpawnPos](Plop::Entity child) {
			if (child.GetComponent<Plop::Component_Name>().sName == "Nozzle")
			{
				vSpawnPos = child.GetComponent<Plop::Component_Transform>().GetWorldPosition();
			}
		});
		{
			Plop::Entity bullet = xLevel->CreateEntity("Bullet");

			Plop::Component_SpriteRenderer &spriteComp = bullet.AddComponent<Plop::Component_SpriteRenderer>();
			spriteComp.xSprite->SetTint(COLOR_RED);

			Component_Bullet &bulletComp = bullet.AddComponent<Component_Bullet>();
			bulletComp.emitting			 = towerEntity;
			bulletComp.vVelocity		 = glm::vec3(bulletComp.fSpeed * vEnemyDir2D, 0.f);

			Plop::Component_AABBCollider &colliderComp = bullet.AddComponent<Plop::Component_AABBCollider>();
			colliderComp.vMin						   = glm::vec3(-0.1f, -0.1f, -10.f);
			colliderComp.vMax						   = glm::vec3(0.1f, 0.1f, 10.f);


			Plop::Component_Transform &transform = bullet.GetComponent<Plop::Component_Transform>();
			transform.SetLocalPosition(vSpawnPos);
			transform.SetLocalRotation(glm::quat(glm::vec3(0.f, 0.f, fAngle)));
			transform.SetLocalScale(glm::vec3(0.2f, 0.2f, 1.f));
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
		}
	};


	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry)
	{
		auto &viewEnemy = _registry.view<Component_Enemy, Plop::Component_Transform>();

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

#ifndef USE_COMPONENT_MGR
namespace MM
{
	template<>
	void ComponentEditorWidget<Component_Tower>(entt::registry &reg, entt::registry::entity_type e)
	{
		auto &comp = reg.get<Component_Tower>(e);
		ImGui::DragFloat("Damage", &comp.fDamage, 0.1f, 1.f);
		ImGui::DragFloat("Firing rate", &comp.fFiringRate, 0.005f, 0.01f, FLT_MAX);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("Fire per seconds, which corresponds to a delay of %.3fs between firing", 1.f / comp.fFiringRate);
			ImGui::EndTooltip();
		}
	}

	template<>
	json ComponentToJson<Component_Tower>(entt::registry &reg, entt::registry::entity_type e)
	{
		auto &comp = reg.get<Component_Tower>(e);
		json  j;
		j["Damage"]		 = comp.fDamage;
		j["Firing rate"] = comp.fFiringRate;
		return j;
	}

	template<>
	void ComponentFromJson<Component_Tower>(entt::registry &reg, entt::registry::entity_type e, const json &_j)
	{
		auto &comp = reg.get_or_emplace<Component_Tower>(e);
		if (_j.contains("Damage"))
			comp.fDamage = _j["Damage"];
		if (_j.contains("Firing rate"))
			comp.fFiringRate = _j["Firing rate"];
	}
} // namespace MM
#endif