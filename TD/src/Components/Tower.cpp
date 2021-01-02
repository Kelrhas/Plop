#include "TD_pch.h"
#include "Tower.h"

#include <ECS/LevelBase.h>
#include <ECS/ECSHelper.h>
#include <Assets/TextureLoader.h>

#include "Components/Bullet.h"

void TowerComponent::Update( const Plop::TimeStep& _ts )
{
	if (fFireDelay > 0.f)
	{
		fFireDelay -= _ts.GetGameDeltaTime();
	}
}

bool TowerComponent::CanFire() const
{
	return fFireDelay <= 0.f;
}

void TowerComponent::Fire( const std::tuple<Plop::Entity, EnemyComponent&, Plop::TransformComponent&>& _enemyData )
{
	fFireDelay += 1.f / fFiringRate;

	Plop::LevelBasePtr xLevel = Plop::LevelBase::GetCurrentLevel().lock();
	Plop::Entity towerEntity = Plop::GetComponentOwner( xLevel, *this );
	Plop::TransformComponent& transformTower = towerEntity.GetComponent<Plop::TransformComponent>();
	glm::vec3 vTowerPos = transformTower.vPosition;
	
	const glm::vec3 vEnemyPos = std::get<2>( _enemyData ).vPosition;
	const glm::vec2 vEnemyDir2D = glm::normalize( vEnemyPos.xy - vTowerPos.xy );
	float fAngle = glm::acos( glm::dot( glm::vec2( 1.f, 0.f ), vEnemyDir2D ) ) - glm::half_pi<float>();
	if (transformTower.vPosition.y > vEnemyPos.y)
		fAngle = glm::pi<float>() - fAngle;
	transformTower.vRotation.z = fAngle;


	Plop::Entity bullet = xLevel->CreateEntity( "Bullet" );

	Plop::SpriteRendererComponent& spriteComp = bullet.AddComponent<Plop::SpriteRendererComponent>();
	spriteComp.xSprite->SetTint( COLOR_RED );

	BulletComponent& bulletComp = bullet.AddComponent<BulletComponent>();
	bulletComp.emitting = towerEntity;
	bulletComp.target = std::get<0>( _enemyData );
	bulletComp.vVelocity = glm::vec3( bulletComp.fSpeed * vEnemyDir2D, 0.f );


	Plop::TransformComponent& transform = bullet.GetComponent<Plop::TransformComponent>();
	transform.vPosition = glm::vec3( vTowerPos.xy, vTowerPos.z - 0.1f );
	transform.vRotation.z = fAngle;
	transform.vScale = glm::vec3( 0.05f, 0.5f, 1.f );
}

namespace MM
{
	template <>
	void ComponentEditorWidget<TowerComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<TowerComponent>( e );
		ImGui::DragFloat( "Damage", &comp.fDamage, 0.1f, 1.f );
		ImGui::DragFloat( "Firing rate", &comp.fFiringRate, 0.1f, 0.01f );
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