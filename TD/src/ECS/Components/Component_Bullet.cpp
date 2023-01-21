#include "TD_pch.h"
#include "Component_Bullet.h"

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

#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>
	void ComponentEditorWidget<Component_Bullet>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Component_Bullet>( e );
		if (ImGui::DragFloat( "Speed", &comp.fSpeed, 0.1f, 1.f ))
		{
			comp.vVelocity = glm::normalize( comp.vVelocity ) * comp.fSpeed;
		}
		ImGui::Text( "Velocity: %.3f %.3f %.3f", comp.vVelocity.x, comp.vVelocity.y, comp.vVelocity.z );
	}

	template <>
	json ComponentToJson<Component_Bullet>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Component_Bullet>( e );
		json j;
		j["Speed"] = comp.fSpeed;
		return j;
	}

	template<>
	void ComponentFromJson<Component_Bullet>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Component_Bullet>( e );
		if (_j.contains( "Speed" ))
			comp.fSpeed = _j["Speed"];
	}
}
#endif