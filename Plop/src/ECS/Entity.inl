
#include "ECS/Components/BaseComponents.h"

namespace Plop
{
#ifdef USE_ENTITY_HANDLE
	template<typename Visitor>
	void Entity::ChildVisitor(Visitor &&visitor) const
	{
		const auto& graphNodeParent = m_hEntity.get<Component_GraphNode>();
		auto childEntity = graphNodeParent.firstChild;
		auto& reg = m_hEntity.registry();

		while (childEntity != entt::null)
		{
			const auto& graphNodeChild = reg.get<Component_GraphNode>(childEntity);
			const auto next = graphNodeChild.nextSibling;
			visitor(Entity(childEntity, reg));

			childEntity = next;
		}
	}


	template <class Comp, typename ...Args>
	Comp& Entity::AddComponent(Args...args)
	{
		ASSERTM(m_hEntity, "Invalid entity");
		return m_hEntity.emplace<Comp>(std::forward<Args>(args)...);
	}

	template <class Comp, typename ...Args>
	void Entity::RemoveComponent()
	{
		ASSERTM(m_hEntity, "Invalid entity");
		return m_hEntity.remove<Comp>();
	}

	template <class Comp>
	bool Entity::HasComponent() const
	{
		if (!m_hEntity)
			return false;

		return m_hEntity.has<Comp>();
	}

	template <class Comp>
	Comp& Entity::GetComponent() const
	{
		ASSERTM(m_hEntity, "Invalid entity");
		ASSERTM(HasComponent<Comp>(), "Entity does not have this component");
		return m_hEntity.get<Comp>();
	}

#else
	template<typename Visitor>
	void Entity::ChildVisitor( Visitor visitor ) const
	{
		const auto& reg = m_xLevel.lock()->m_ENTTRegistry;
		const auto& graphNodeParent = reg.get<Component_GraphNode>( m_EntityId );
		auto childEntity = graphNodeParent.firstChild;

		while (childEntity != entt::null)
		{
			visitor( Entity{ childEntity, m_xLevel } );

			const auto& graphNodeChild = reg.get<Component_GraphNode>( childEntity );
			childEntity = reg.get<Component_GraphNode>( childEntity ).nextSibling;
		}
	}


	template <class Comp, typename ...Args>
	Comp& Entity::AddComponent( Args...args )
	{
		ASSERTM( !m_xLevel.expired(), "No scene associated with entity" );
		return m_xLevel.lock()->m_ENTTRegistry.emplace<Comp>( m_EntityId, std::forward<Args>( args )... );
	}

	template <class Comp, typename ...Args>
	void Entity::RemoveComponent()
	{
		ASSERTM( !m_xLevel.expired(), "No scene associated with entity" );
		return m_xLevel.lock()->m_ENTTRegistry.remove<Comp>( m_EntityId );
	}

	template <class Comp>
	bool Entity::HasComponent() const
	{
		if (m_xLevel.expired())
			return false;

		return m_xLevel.lock()->m_ENTTRegistry.has<Comp>( m_EntityId );
	}

	template <class Comp>
	Comp& Entity::GetComponent() const
	{
		ASSERTM( !m_xLevel.expired(), "No scene associated with entity" );
		ASSERTM( HasComponent<Comp>(), "Entity does not have this component" );
		return m_xLevel.lock()->m_ENTTRegistry.get<Comp>( m_EntityId );
	}


#endif
}