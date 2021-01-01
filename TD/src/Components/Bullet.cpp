#include "TD_pch.h"
#include "Bullet.h"


BulletComponent::BulletComponent( const BulletComponent& _other )
{
	fSpeed = _other.fSpeed;
	vVelocity = _other.vVelocity;
	emitting = _other.emitting;
	target = _other.target;
}

namespace MM
{
	template <>
	void ComponentEditorWidget<BulletComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<BulletComponent>( e );
		if (ImGui::DragFloat( "Speed", &comp.fSpeed, 0.1f, 1.f ))
		{
			comp.vVelocity = glm::normalize( comp.vVelocity ) * comp.fSpeed;
		}
		ImGui::Text( "Velocity: %.3f %.3f %.3f", comp.vVelocity.x, comp.vVelocity.y, comp.vVelocity.z );
	}

	template <>
	json ComponentToJson<BulletComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<BulletComponent>( e );
		json j;
		j["Speed"] = comp.fSpeed;
		return j;
	}

	template<>
	void ComponentFromJson<BulletComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<BulletComponent>( e );
		if (_j.contains( "Speed" ))
			comp.fSpeed = _j["Speed"];
	}
}