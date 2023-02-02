#pragma once

#include <ECS/ComponentManager.h>

namespace Plop
{
	struct Component_AABBCollider
	{
		glm::vec3 vMin = -VEC3_1 / 2.f;
		glm::vec3 vMax = VEC3_1 / 2.f;

		bool IsInside( const glm::vec3& _vPoint ) const;
		bool IsColliding( const Component_AABBCollider& _o, const glm::vec3& _vCenter ) const;

		void EditorUI();
		json ToJson() const;
		void FromJson( const json& _j );
	};
}