#pragma once


#ifdef USE_COMPONENT_MGR
#include <ECS/ComponentManager.h>
#else
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>
#endif

#include <Camera/Camera.h>

namespace Plop
{
	struct Component_Camera
	{
		CameraPtr xCamera = nullptr;


		void EditorUI();
		json ToJson() const;
		void FromJson( const json& _json );
		Component_Camera& operator=( const Component_Camera& _other );

	};
}


#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>	void ComponentEditorWidget<Plop::Component_Camera>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::Component_Camera>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::Component_Camera>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}
#endif