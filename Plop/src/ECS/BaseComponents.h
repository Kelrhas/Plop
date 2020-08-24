#pragma once

#include <Renderer/Sprite.h>
#include <Camera/Camera.h>

#include <imgui_entt_entity_editor.hpp>

namespace Plop
{
	struct NameComponent
	{
		NameComponent() = default;
		NameComponent( const String& _sName ) : sName( _sName ) {}

		String sName;
	};

	struct GraphNodeComponent
	{
		// https://skypjack.github.io/2019-06-25-ecs-baf-part-4/
		static const size_t MAX_CHILDREN = 8;

		entt::entity parent{ entt::null };
		std::array<entt::entity, MAX_CHILDREN> children{};
		size_t nbChild = 0;
	};

	struct TransformComponent
	{
		TransformComponent() = default;
		TransformComponent( const glm::mat4& _mTransform ) : mTransform( _mTransform ) {}
		// TRS constructor
		TransformComponent( const glm::vec3& _vTranslation, const glm::quat& _qRotation, const glm::vec3& _vScale )
		{
			Debug::TODO( "test this" );
			mTransform = glm::translate( glm::identity<glm::mat4>(), _vTranslation );
			mTransform = mTransform * (glm::mat4)_qRotation;
			mTransform = glm::scale( mTransform, _vScale );
		}

		operator glm::mat4() const { return mTransform; }

		glm::vec3 GetPosition() const { return mTransform[3]; }
		void SetPosition( const glm::vec3& _vPos ) { mTransform[3] = glm::vec4( _vPos, mTransform[3][3] ); }

		glm::mat4 mTransform = glm::identity<glm::mat4>();
	};

	struct SpriteRendererComponent
	{
		SpritePtr xSprite = nullptr;
	};

	struct CameraComponent
	{
		CameraPtr xCamera = nullptr;
	};
}

namespace MM
{
	template <>
	void ComponentEditorWidget<Plop::NameComponent>( entt::registry& reg, entt::registry::entity_type e );

	template <>
	void ComponentEditorWidget<Plop::TransformComponent>( entt::registry& reg, entt::registry::entity_type e );

	template <>
	void ComponentEditorWidget<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e );

	template <>
	void ComponentEditorWidget<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e );
}