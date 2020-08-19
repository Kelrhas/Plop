#include "Plop_pch.h"
#include "Entity.h"

#include <imgui_entt_entity_editor.hpp>


namespace MM {
	template <>
	void ComponentEditorWidget<glm::mat4>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& t = reg.get<glm::mat4>( e );
		ImGui::DragFloat4( "0", &t[0].x, 0.1f );
		ImGui::DragFloat4( "1", &t[1].x, 0.1f );
		ImGui::DragFloat4( "2", &t[2].x, 0.1f );
		ImGui::DragFloat4( "3", &t[3].x, 0.1f );
	}
}

namespace Plop
{
	::MM::EntityEditor<entt::entity> editor;

	Entity::Entity( entt::entity _entityID, const LevelWeakPtr& _xLevel )
		: m_EntityId( _entityID )
		, m_xLevel( _xLevel )
	{}

	Entity::Entity( Entity&& _other ) noexcept
	{
		m_EntityId = _other.m_EntityId;
		m_xLevel = _other.m_xLevel;

		// make sure the other entity is no longer valid
		_other.m_EntityId = entt::null;
		_other.m_xLevel.reset(); 

		editor.registerComponent<glm::mat4>( "Transform" );
	}


	Entity::operator bool() const
	{
		if (m_EntityId == entt::null || m_xLevel.expired() )
			return false;

		return m_xLevel.lock()->m_ENTTRegistry.valid( m_EntityId );
	}

	Entity& Entity::operator=( const Entity& _other )
	{
		m_EntityId = _other.m_EntityId;
		m_xLevel = _other.m_xLevel;

		return *this;
	}


	void Entity::ImGuiDraw()
	{
		editor.render( m_xLevel.lock()->m_ENTTRegistry, m_EntityId );
	}
}
