#pragma once

#include <ECS/ComponentManager.h>
#include "ECS/Components/ComponentDefinition.h"

namespace Plop
{
	struct Component_Transform
	{
		Component_Transform() = default;
		Component_Transform( const glm::vec3& _vTranslation, const glm::vec3& _vRotation = VEC3_0, const glm::vec3& _vScale = VEC3_1 )
			: vPosition( _vTranslation ), qRotation( glm::quat( _vRotation ) ), vScale( _vScale )
		{}
		Component_Transform( const glm::vec3& _vTranslation, const glm::quat& _qRotation = glm::identity<glm::quat>(), const glm::vec3& _vScale = VEC3_1 )
			: vPosition( _vTranslation ), qRotation( _qRotation ), vScale( _vScale )
		{}


		void EditorUI();
		json ToJson() const;
		void FromJson( const json& _j );



		float Distance2D( const Component_Transform& _other ) const;
		float Distance2DSquare( const Component_Transform& _other ) const;

		// LOCAL
		void SetLocalMatrix( const glm::mat4& _m );
		glm::mat4 GetLocalMatrix() const;

		void SetLocalPosition( const glm::vec3& _vPos ) { vPosition = _vPos; }
		glm::vec3 GetLocalPosition() const { return vPosition; }

		void SetLocalRotation( const glm::quat& _qRot ) { qRotation = _qRot; }
		glm::quat GetLocalRotation() const { return qRotation; }


		void SetLocalScale( const glm::vec3& _vScale ) { vScale = _vScale; }
		glm::vec3 GetLocalScale() const { return vScale; }

		void TranslateLocal( const glm::vec3& _vTrans );


		// WORLD
		void SetWorldMatrix( const glm::mat4& _m );
		glm::mat4 GetWorldMatrix() const;

		void SetWorldPosition( const glm::vec3& _vPos );
		glm::vec3 GetWorldPosition() const;

		void SetWorldRotation( const glm::quat& _qRot );
		glm::quat GetWorldRotation() const;

		void TranslateWorld( const glm::vec3& _vTrans );






		//operator glm::mat4() const { return GetLocalMatrix(); }
		//operator glm::quat() const { return glm::quat( vRotation ); }

	private:
		glm::vec3 vPosition = glm::vec3( 0.f, 0.f, 0.f );
		glm::quat qRotation = glm::identity<glm::quat>();
		glm::vec3 vScale = glm::vec3( 1.f, 1.f, 1.f );
	};

	template<>
	constexpr bool CanRemoveComponent<Component_Transform>() { return false; }
}
