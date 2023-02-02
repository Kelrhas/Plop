#include "Plop_pch.h"
#include "Component_Physics.h"

#include <glm/gtc/type_ptr.hpp>

#include "Application.h"
#include "Utils/JsonTypes.h"
#include "Editor/EditorLayer.h"
#include "ECS/Components/Component_Transform.h"
#include "ECS/ECSHelper.h"


namespace Plop
{
	bool Component_AABBCollider::IsInside( const glm::vec3& _vPoint ) const
	{
		auto xLevel = Application::GetCurrentLevel().lock();
#ifdef USE_ENTITY_HANDLE
		auto owner = GetComponentOwner( xLevel->GetEntityRegistry(), *this );
#else
		auto owner = GetComponentOwner( xLevel, *this );
#endif

		const glm::vec3& vWorldPos = owner.GetComponent<Component_Transform>().GetWorldPosition();
		glm::vec3 vLocalPoint = _vPoint - vWorldPos;

		return (vLocalPoint.x >= vMin.x && vLocalPoint.x <= vMax.x) &&
			(vLocalPoint.y >= vMin.y && vLocalPoint.y <= vMax.y) &&
			(vLocalPoint.z >= vMin.z && vLocalPoint.z <= vMax.z);
	}

	bool Component_AABBCollider::IsColliding( const Component_AABBCollider& _o, const glm::vec3& _vCenter ) const
	{
		auto xLevel = Application::GetCurrentLevel().lock();
#ifdef USE_ENTITY_HANDLE
		auto owner = GetComponentOwner(xLevel->GetEntityRegistry(), *this);
#else
		auto owner = GetComponentOwner(xLevel, *this);
#endif

		const glm::vec3& vWorldPos = owner.GetComponent<Component_Transform>().GetWorldPosition();
		glm::vec3 vWorldMinA = vMin + vWorldPos;
		glm::vec3 vWorldMaxA = vMax + vWorldPos;
		glm::vec3 vWorldMinB = _o.vMin + _vCenter;
		glm::vec3 vWorldMaxB = _o.vMax + _vCenter;


		return (vWorldMinA.x <= vWorldMaxB.x && vWorldMaxA.x >= vWorldMinB.x) &&
			(vWorldMinA.y <= vWorldMaxB.y && vWorldMaxA.y >= vWorldMinB.y) &&
			(vWorldMinA.z <= vWorldMaxB.z && vWorldMaxA.z >= vWorldMinB.z);
	}

	void Component_AABBCollider::EditorUI()
	{
		ImGui::DragFloat3( "Min", glm::value_ptr( vMin ), 0.1f );
		ImGui::DragFloat3( "Max", glm::value_ptr( vMax ), 0.1f );


		auto xLevel = Application::GetCurrentLevel().lock();
#ifdef USE_ENTITY_HANDLE
		auto owner = GetComponentOwner(xLevel->GetEntityRegistry(), *this);
#else
		auto owner = GetComponentOwner(xLevel, *this);
#endif
		const auto& transform = owner.GetComponent<Component_Transform>();
		const glm::vec3& vWorldPos = transform.GetWorldPosition();
		constexpr glm::vec3 vColliderColor(0.1f, 0.8f, 0.15f);
		Plop::EditorGizmo::AABB(vMin + vWorldPos, vMax + vWorldPos, vColliderColor);
	}

	json Component_AABBCollider::ToJson() const
	{
		json j;
		j["Min"] = vMin;
		j["Max"] = vMax;
		return j;
	}

	void Component_AABBCollider::FromJson(const json& _j )
	{
		if (_j.contains( "Min" ))
			vMin = _j["Min"];

		if (_j.contains( "Max" ))
			vMax = _j["Max"];
	}
}
