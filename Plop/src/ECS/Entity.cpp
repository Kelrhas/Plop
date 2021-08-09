#include "Plop_pch.h"
#include "Entity.h"

#include <ECS/Components/BaseComponents.h>
#include <Utils/JsonTypes.h>
#include <Editor/EditorLayer.h>

namespace Plop
{
	Entity::Entity( entt::null_t )
		: m_EntityId( entt::null )
	{
		m_xLevel.reset();
	}

	Entity::Entity( entt::entity _entityID, const LevelBaseWeakPtr& _xLevel )
		: m_EntityId( _entityID )
		, m_xLevel( _xLevel )
	{}

	Entity::Entity( const Entity& _other ) noexcept
	{
		m_EntityId = _other.m_EntityId;
		m_xLevel = _other.m_xLevel;
	}

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

	void Entity::Destroy()
	{
		auto xLevel = m_xLevel.lock();
		xLevel->DestroyEntity( *this );
		Reset();
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
				auto& graphNode = reg.get<Component_GraphNode>( m_EntityId );

				parent.m_EntityId = graphNode.parent;
			}
		}

		return parent;
	}

	void Entity::SetParent( Entity& _Parent )
	{
		ASSERTM( m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity" );

		auto& reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid( m_EntityId ))
		{
			auto& graphNode = reg.get<Component_GraphNode>( m_EntityId );

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

	void Entity::GetChildren(std::vector<Entity>& _outvecChildren) const
	{
		_outvecChildren.clear();

		if (m_EntityId != entt::null && !m_xLevel.expired())
		{
			auto& reg = m_xLevel.lock()->m_ENTTRegistry;
			if (reg.valid( m_EntityId ))
			{
				auto& graphNodeParent = reg.get<Component_GraphNode>( m_EntityId );

#ifdef ALLOW_DYNAMIC_CHILDREN
				auto childEntity = graphNodeParent.firstChild;
				while(childEntity != entt::null)
				{
					_outvecChildren.emplace_back( childEntity, m_xLevel );
					childEntity = reg.get<Component_GraphNode>( childEntity ).nextSibling;
				}
#else
				for (int i = 0; i < graphNodeParent.nbChild; ++i)
				{
					_outvecChildren.emplace_back( graphNodeParent.children[i], m_xLevel );
				}
#endif
			}
		}
	}

	void to_json( json& j, const Entity& e ) {
		j = e.ToJson();
	}

	json Entity::ToJson() const
	{
		json j = EditorLayer::GetJsonEntity( *this );

		j["HintID"] = m_EntityId;
		j["Name"] = GetComponent<Component_Name>().sName;
		static std::vector<Entity> vecChildren;
		GetChildren( vecChildren );
		j["Children"] = vecChildren;

		return j;
	}

	void Entity::FromJson( const json& _jEntity )
	{
		GetComponent<Component_Name>().sName = _jEntity["Name"];

		if (_jEntity.contains( "Children" ))
		{
			LevelBasePtr xLevel = LevelBase::GetCurrentLevel().lock();
			ASSERTM( xLevel != nullptr, "No current level" );
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
		ASSERTM( m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity" );
		ASSERTM( _Child, "Invalid Entity for _Child" );

		auto& reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid( m_EntityId ))
		{
			auto& graphNodeParent = reg.get<Component_GraphNode>( m_EntityId );

#ifdef ALLOW_DYNAMIC_CHILDREN
			if (graphNodeParent.firstChild != entt::null)
			{
				auto& graphNodeChild = reg.get<Component_GraphNode>( graphNodeParent.firstChild );
				graphNodeChild.prevSibling = _Child;

				auto& graphNodeNewChild = reg.get<Component_GraphNode>( _Child );
				graphNodeNewChild.nextSibling = graphNodeParent.firstChild;
			}
			
			graphNodeParent.firstChild = _Child;
#else
			ASSERTM( graphNodeParent.nbChild + 1 < Component_GraphNode::MAX_CHILDREN, "No room for another child" );
			if (graphNodeParent.nbChild + 1 < graphNodeParent.MAX_CHILDREN)
			{
				graphNodeParent.children[graphNodeParent.nbChild++] = _Child.m_EntityId;
			}
#endif
		}
	}

	void Entity::RemoveChild( Entity& _Child )
	{
		ASSERTM( m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity" );
		ASSERTM( _Child, "Invalid Entity for _Child" );

		auto& reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid( m_EntityId ))
		{

			auto& graphNodeParent = reg.get<Component_GraphNode>( m_EntityId );

#ifdef ALLOW_DYNAMIC_CHILDREN

			const auto& graphNodeChild = reg.get<Component_GraphNode>( _Child );

			if (graphNodeParent.firstChild == _Child.m_EntityId)
			{
				graphNodeParent.firstChild = graphNodeChild.nextSibling;
			}
			else //if (graphNodeChild.prevSibling != entt::null)
			{
				auto& graphNodePrev = reg.get<Component_GraphNode>( graphNodeChild.prevSibling );
				graphNodePrev.nextSibling = graphNodeChild.nextSibling;
			}
			if (graphNodeChild.nextSibling != entt::null)
			{
				auto& graphNodeNext = reg.get<Component_GraphNode>( graphNodeChild.nextSibling );
				graphNodeNext.prevSibling = graphNodeChild.prevSibling;
			}

#else
			for (int i = 0; i < graphNodeParent.nbChild; ++i)
			{
				if (graphNodeParent.children[i] == _Child.m_EntityId)
				{
					graphNodeParent.children[i] = std::move( graphNodeParent.children[graphNodeParent.nbChild - 1] );
					graphNodeParent.nbChild--;
					break;
				}
			}
#endif
		}
	}
#pragma endregion
}
