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
	// TODO: merge GraphNode & Transform ?

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
	template <>	void ComponentEditorWidget<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::SpriteRendererComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );

	template <>	void ComponentEditorWidget<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::CameraComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}