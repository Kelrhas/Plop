#include "Plop_pch.h"
#include "Component_Physics.h"

#include <glm/gtc/type_ptr.hpp>

#include "Utils/JsonTypes.h"
#include "Editor/EditorLayer.h"
#include "ECS/Components/Component_Transform.h"
#include "ECS/ECSHelper.h"


namespace Plop
{
	bool Component_AABBCollider::IsInside( const glm::vec3& _vPoint ) const
	{
		auto xLevel = LevelBase::GetCurrentLevel().lock();
		auto owner = GetComponentOwner( xLevel, *this );

		const glm::vec3& vWorldPos = owner.GetComponent<Component_Transform>().GetWorldPosition();
		glm::vec3 vLocalPoint = _vPoint - vWorldPos;

		return (vLocalPoint.x >= vMin.x && vLocalPoint.x <= vMax.x) &&
			(vLocalPoint.y >= vMin.y && vLocalPoint.y <= vMax.y) &&
			(vLocalPoint.z >= vMin.z && vLocalPoint.z <= vMax.z);
	}

	bool Component_AABBCollider::IsColliding( const Component_AABBCollider& _o, const glm::vec3& _vCenter ) const
	{
		auto xLevel = LevelBase::GetCurrentLevel().lock();
		auto owner = GetComponentOwner( xLevel, *this );

		const glm::vec3& vWorldPos = owner.GetComponent<Component_Transform>().GetWorldPosition();
		glm::vec3 vWorldMinA = vMin + vWorldPos;
		glm::vec3 vWorldMaxA = vMax + vWorldPos;
		glm::vec3 vWorldMinB = _o.vMin + _vCenter;
		glm::vec3 vWorldMaxB = _o.vMax + _vCenter;


		return (vWorldMinA.x <= vWorldMaxB.x && vWorldMaxA.x >= vWorldMinB.x) &&
			(vWorldMinA.y <= vWorldMaxB.y && vWorldMaxA.y >= vWorldMinB.y) &&
			(vWorldMinA.z <= vWorldMaxB.z && vWorldMaxA.z >= vWorldMinB.z);
	}
}

namespace MM
{
	template <>
	void ComponentEditorWidget<Plop::Component_AABBCollider>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::Component_AABBCollider>( e );
		ImGui::DragFloat3( "Min", glm::value_ptr( comp.vMin ), 0.1f );
		ImGui::DragFloat3( "Max", glm::value_ptr( comp.vMax ), 0.1f );


		const auto& transform = reg.get<Plop::Component_Transform>( e );
		const glm::vec3& vWorldPos = transform.GetWorldPosition();
		Plop::EditorGizmo::AABB( comp.vMin + vWorldPos, comp.vMax + vWorldPos );
	}

	template <>
	json ComponentToJson<Plop::Component_AABBCollider>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::Component_AABBCollider>( e );
		json j;
		j["Min"] = comp.vMin;
		j["Max"] = comp.vMax;
		return j;
	}

	template<>
	void ComponentFromJson<Plop::Component_AABBCollider>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Plop::Component_AABBCollider>( e );

		if (_j.contains( "Min" ))
			comp.vMin = _j["Min"];

		if (_j.contains( "Max" ))
			comp.vMax = _j["Max"];
	}
}