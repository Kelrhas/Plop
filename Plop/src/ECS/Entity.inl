
namespace Plop
{
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
}