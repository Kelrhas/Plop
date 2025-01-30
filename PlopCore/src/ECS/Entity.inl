
#include "ECS/Components/BaseComponents.h"

namespace Plop
{
	template<typename Visitor> requires VisitorConcept<Visitor, Entity>
	void Entity::VisitChildren(Visitor &&visitor) const
	{
		const auto& graphNodeParent = m_hEntity.get<Component_GraphNode>();
		auto childEntity = graphNodeParent.firstChild;
		entt::registry &reg				= *m_hEntity.registry();

		while (childEntity != entt::null)
		{
			const auto& graphNodeChild = reg.get<Component_GraphNode>(childEntity);
			const auto next = graphNodeChild.nextSibling;
			if (visitor(Entity(childEntity, reg)) == VisitorFlow::BREAK)
				break;

			childEntity = next;
		}
	}

	template<typename Visitor> requires VisitorConcept<Visitor, Entity>
	void Entity::VisitChildrenRecursive(Visitor &&visitor) const
	{
		/*
			Depth First Search, like so

				1
			   / \
			  2   8
			 / \
			3   7
		   /
		  4
		 / \
		5   6
		*/

		// first, call the visitor on this entity
		if (visitor(*this) == VisitorFlow::BREAK)
			return;

		const entt::entity rootEntity  = m_hEntity.entity();
		entt::entity	   childEntity = m_hEntity.get<Component_GraphNode>().firstChild;
		entt::registry	  &reg		   = *m_hEntity.registry();

		while (childEntity != entt::null)
		{
			if (visitor(Entity { childEntity, reg }) == VisitorFlow::BREAK)
				break;
			
			const Component_GraphNode& graphNodeChild = reg.get<Component_GraphNode>(childEntity);
			if(graphNodeChild.firstChild != entt::null)
				childEntity = graphNodeChild.firstChild;
			else if (graphNodeChild.nextSibling != entt::null)
				childEntity = graphNodeChild.nextSibling;
			else
			{
				childEntity = graphNodeChild.parent;
				entt::entity next = entt::null;
				while(next == entt::null)
				{
					const Component_GraphNode& graphNodeParent = reg.get<Component_GraphNode>(childEntity);
					if (childEntity == rootEntity)
					{
						break; // visit finished since we do not have any more parent
					}
					next = graphNodeParent.nextSibling;
					childEntity = graphNodeParent.parent;
				}
				childEntity = next;
			}
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
		m_hEntity.remove<Comp>();
	}

	template <class Comp>
	bool Entity::HasComponent() const
	{
		if (!m_hEntity)
			return false;

		return m_hEntity.all_of<Comp>();
	}

	template <class Comp>
	Comp& Entity::GetComponent() const
	{
		ASSERTM(m_hEntity, "Invalid entity");
		ASSERTM(HasComponent<Comp>(), "Entity does not have this component");
		return m_hEntity.get<Comp>();
	}

	template<class Comp>
	Comp *Entity::GetComponentInHierarchy() const
	{
		ASSERTM(m_hEntity, "Invalid entity");

		if (m_hEntity.all_of<Comp>())
			return &m_hEntity.get<Comp>();

		Comp *pComp = nullptr;
		VisitChildrenRecursive(
		  [&](Entity _e)
		  {
			  if (_e.HasComponent<Comp>())
			  {
				  pComp = &_e.GetComponent<Comp>();
				  return VisitorFlow::BREAK;
			  }
			  return VisitorFlow::CONTINUE;
		  });


		return pComp;
	}
	
}