#pragma once


#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>

#include <Camera/Camera.h>

namespace Plop
{
	struct Component_Camera
	{
		Component_Camera& operator=( const Component_Camera& _other );

		CameraPtr xCamera = nullptr;
	};
}


namespace MM
{
	template <>	void ComponentEditorWidget<Plop::Component_Camera>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::Component_Camera>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::Component_Camera>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}