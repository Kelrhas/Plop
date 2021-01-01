#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>

#include <Renderer/Sprite.h>
#include <Camera/Camera.h>


namespace Plop
{
	struct NameComponent
	{
		NameComponent( const String& _sName = "Entity" ) : sName( _sName ) {}

		String sName;
	};

	struct GraphNodeComponent
	{
		// https://skypjack.github.io/2019-06-25-ecs-baf-part-4/
		static const size_t MAX_CHILDREN = 8;

		entt::entity parent{ entt::null };
		std::array<entt::entity, MAX_CHILDREN> children{}; // cannot be a dynamic sized array
		size_t nbChild = 0;
	};

	struct TransformComponent
	{
		TransformComponent() = default;
		TransformComponent( const glm::vec3& _vTranslation, const glm::vec3& _vRotation = VEC3_0, const glm::vec3& _vScale = VEC3_1 )
			: vPosition( _vTranslation ), vRotation( _vRotation ), vScale( _vScale )
		{}


		glm::mat4 GetMatrix() const;
		float Distance2D( const TransformComponent& _other ) const;
		float Distance2DSquare( const TransformComponent& _other ) const;

		operator glm::mat4() const { return GetMatrix(); }
		operator glm::quat() const { return glm::quat( vRotation ); }

		glm::vec3 vPosition = glm::vec3( 0.f, 0.f, 0.f );
		glm::vec3 vRotation = glm::vec3( 0.f, 0.f, 0.f );
		glm::vec3 vScale = glm::vec3( 1.f, 1.f, 1.f );
	};

	struct SpriteRendererComponent
	{
		SpriteRendererComponent();
		SpriteRendererComponent& operator=( const SpriteRendererComponent& _other ); 

		SpritePtr xSprite = nullptr;
	};

	struct CameraComponent
	{
		CameraComponent& operator=( const CameraComponent& _other );

		CameraPtr xCamera = nullptr;
	};
}

namespace MM
{
	template <>	void ComponentEditorWidget<Plop::TransformComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::TransformComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::TransformComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );

	template <>	void ComponentEditorWidget<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );

	template <>	void ComponentEditorWidget<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}