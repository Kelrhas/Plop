
#include "ECS/Components/BaseComponents.h"

namespace Plop
{
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
			if (visitor(Entity(childEntity, reg)) == VisitorFlow::BREAK)
				break;

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
	
}