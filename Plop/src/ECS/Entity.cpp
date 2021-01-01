#include "Plop_pch.h"
#include "Entity.h"

#include <ECS/BaseComponents.h>
#include <Utils/JsonTypes.h>
#include <Editor/EditorLayer.h>

namespace Plop
{
	Entity::Entity( entt::entity _entityID, const LevelBaseWeakPtr& _xLevel )
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

	void Entity::Reset()
	{
		m_EntityId = entt::null;
		m_xLevel.reset();
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

	bool Entity::operator==( const Entity& _other )
	{
		return m_EntityId == _other.m_EntityId && (!m_xLevel.expired() && !_other.m_xLevel.expired() && m_xLevel.lock() == _other.m_xLevel.lock());
	}

	bool Entity::operator!=( const Entity& _other )
	{
		return !operator==(_other);
	}

	Entity Entity::GetParent() const
	{
		Entity parent(entt::null, m_xLevel);

		if (m_EntityId != entt::null && !m_xLevel.expired())
		{
			auto& reg = m_xLevel.lock()->m_ENTTRegistry;
			if (reg.valid( m_EntityId ))
			{
				auto& graphNode = reg.get<GraphNodeComponent>( m_EntityId );

				parent.m_EntityId = graphNode.parent;
			}
		}

		return parent;
	}

	void Entity::SetParent( Entity& _Parent )
	{
		ASSERT( m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity" );

		auto& reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid( m_EntityId ))
		{
			auto& graphNode = reg.get<GraphNodeComponent>( m_EntityId );

			Entity oldParent( graphNode.parent, m_xLevel );
			if (oldParent != _Parent)
			{
				if (oldParent)
				{
					oldParent.RemoveChild( *this );
				}
				graphNode.parent = _Parent.m_EntityId;

				if (_Parent)
				{
					_Parent.AddChild( *this );
				}
			}
		}
	}

	std::vector<Entity> Entity::GetChildren() const
	{
		std::vector<Entity> vecChildren;

		if (m_EntityId != entt::null && !m_xLevel.expired())
		{
			auto& reg = m_xLevel.lock()->m_ENTTRegistry;
			if (reg.valid( m_EntityId ))
			{
				auto& graphNode = reg.get<GraphNodeComponent>( m_EntityId );

				for (int i = 0; i < graphNode.nbChild; ++i)
				{
					vecChildren.emplace_back( graphNode.children[i], m_xLevel );
				}
			}
		}

		return vecChildren;
	}

	void to_json( json& j, const Entity& e ) {
		j = e.ToJson();
	}

	json Entity::ToJson() const
	{
		json j = EditorLayer::GetJsonEntity( *this );

		j["HintID"] = m_EntityId;
		j["Name"] = GetComponent<NameComponent>().sName;
		j["Children"] = GetChildren();

		return j;
	}

	void Entity::FromJson( const json& _jEntity )
	{
		GetComponent<NameComponent>().sName = _jEntity["Name"];

		if (_jEntity.contains( "Children" ))
		{
			LevelBasePtr xLevel = LevelBase::GetCurrentLevel().lock();
			ASSERT( xLevel != nullptr, "No current level" );
			for (auto& j : _jEntity["Children"])
			{
				Entity e = xLevel->CreateEntity();
				e.SetParent( *this );
				e.FromJson( j );
			}
		}
		EditorLayer::SetJsonEntity( *this, _jEntity );
	}

#pragma region private

	void Entity::AddChild( Entity& _Child )
	{
		ASSERT( m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity" );
		ASSERT( _Child, "Invalid Entity for _Child" );

		auto& reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid( m_EntityId ))
		{
			auto& graphNode = reg.get<GraphNodeComponent>( m_EntityId );
			ASSERT( graphNode.nbChild + 1 < GraphNodeComponent::MAX_CHILDREN, "No room for another child" );
			if (graphNode.nbChild + 1 < graphNode.MAX_CHILDREN)
			{
				graphNode.children[graphNode.nbChild++] = _Child.m_EntityId;
			}
		}
	}

	void Entity::RemoveChild( Entity& _Child )
	{
		ASSERT( m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity" );
		ASSERT( _Child, "Invalid Entity for _Child" );

		auto& reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid( m_EntityId ))
		{
			auto& graphNode = reg.get<GraphNodeComponent>( m_EntityId );
			for (int i = 0; i < graphNode.nbChild; ++i)
			{
				if (graphNode.children[i] == _Child.m_EntityId)
				{
					graphNode.children[i] = std::move( graphNode.children[graphNode.nbChild - 1] );
					graphNode.nbChild--;
					break;
				}
			}
		}
	}
#pragma endregion
}
