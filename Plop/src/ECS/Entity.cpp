#include "Plop_pch.h"
#include "Entity.h"


namespace Plop
{
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
}
