#include "TD_pch.h"
#include "Component_Bullet.h"

#include "Component_Enemy.h"
#include "Component_Tower.h"
#include "ECS/TDLevel.h"

#include <ECS/Components/Component_ParticleSystem.h>
#include <ECS/Components/Component_Physics.h>
#include <ECS/Components/Component_Transform.h>

static constexpr const char *JSON_SPEED = "Speed";

Component_Bullet::Component_Bullet( const Component_Bullet& _other )
{
	fSpeed = _other.fSpeed;
	vVelocity = _other.vVelocity;
	emitting = _other.emitting;
}

void Component_Bullet::EditorUI()
{
	ImGui::DragFloat("Speed", &fSpeed, 0.001f, 100.f);
}

json Component_Bullet::ToJson() const
{
	json j;
	j[JSON_SPEED] = fSpeed;
	return j;
}
void Component_Bullet::FromJson(const json &_j)
{
	if (_j.contains(JSON_SPEED))
		fSpeed = _j[JSON_SPEED];
}

namespace BulletSystem
{
	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry)
	{
		PROFILING_FUNCTION();
		Plop::LevelBasePtr xLevel	  = Plop::Application::GetCurrentLevel().lock();
		auto viewEnemy = _registry.view<Component_Enemy, Plop::Component_Transform, Plop::Component_AABBCollider>();

		//float fMaxDistSq = glm::compMax(Plop::Application::Get()->GetWindow().GetViewportSize()) * 2.f;
		constexpr float fMaxDistSq = 20.f;

		_registry.view<Component_Bullet, Plop::Component_Transform, Plop::Component_AABBCollider>().each(
		  [&](entt::entity entityID, const Component_Bullet &bulletComp, Plop::Component_Transform &transform, const Plop::Component_AABBCollider &collider)
		  {
			  transform.TranslateWorld(bulletComp.vVelocity * _ts.GetGameDeltaTime());

			  bool bDestroyBullet = false;

			  const glm::vec2 &thisPos2D = transform.GetWorldPosition();

			  {
				  viewEnemy.each(
					[&](entt::entity						entityID,
						Component_Enemy					   &enemyComp,
						const Plop::Component_Transform	   &transformEnemy,
						const Plop::Component_AABBCollider &colliderEnemy)
					{
						if (!enemyComp.IsDead() && !bDestroyBullet)
						{
							if (collider.IsColliding(colliderEnemy, transformEnemy.GetWorldPosition()))
							{
								bDestroyBullet = true;
								Plop::Entity enemy(entityID, _registry);
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
				  xLevel->DestroyEntity(Plop::Entity(entityID, _registry));
			  }
		  });
	}
} // namespace BulletSystem