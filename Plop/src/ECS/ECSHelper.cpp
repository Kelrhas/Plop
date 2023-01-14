#include "Plop_pch.h"
#include "ECSHelper.h"

#include <queue>
#include <entt/entt.hpp>

#include "ECS/Components/BaseComponents.h"

namespace Plop
{
	entt::entity CopyEntityHierarchyToRegistry(entt::handle _hSrc, entt::registry &regDst, EntityMapping &_mapping, entt::entity _rootDst /*= entt::null*/)
	{
		if (!_hSrc)
			return entt::null;

		_mapping.clear();

		auto &regSrc = _hSrc.registry();

		std::queue<entt::entity> todo;

		auto CopyEntity = [&todo, &regSrc, &regDst](entt::entity enttIDSrc) -> entt::entity {

			entt::entity enttIDDst = regDst.create();

			// Copy components
			regSrc.visit([&regSrc, &regDst, enttIDSrc, enttIDDst](const auto compId) {
				const entt::meta_type type = entt::resolve_type(compId);
				type.func("clone"_hs).invoke({}, std::ref(regSrc), enttIDSrc, std::ref(regDst), enttIDDst);
			});

			// Add the children to the queue
			const auto &graphNodeSrc = regSrc.get<Component_GraphNode>(enttIDSrc);
			entt::entity childEntity = graphNodeSrc.firstChild;
			while (childEntity != entt::null)
			{
				todo.push(childEntity);

				childEntity = regSrc.get<Component_GraphNode>(childEntity).nextSibling;
			}

			return enttIDDst;
		};

		// do the first one to retrieve the root id
		entt::entity parentDst = _rootDst;
		if (parentDst == entt::null)
			parentDst = CopyEntity(_hSrc.entity());
		else
		{
			regSrc.visit([&regSrc, &regDst, enttIDSrc = _hSrc.entity(), enttIDDst=parentDst](const auto compId) {
				const entt::meta_type type = entt::resolve_type(compId);
				type.func("clone"_hs).invoke({}, std::ref(regSrc), enttIDSrc, std::ref(regDst), enttIDDst);
			});

			const auto & graphNodeSrc = _hSrc.get<Component_GraphNode>();
			entt::entity childEntity = graphNodeSrc.firstChild;
			while (childEntity != entt::null)
			{
				todo.push(childEntity);

				childEntity = regSrc.get<Component_GraphNode>(childEntity).nextSibling;
			}
		}
		_mapping[_hSrc.entity()] = parentDst;

		// then all the rest
		while (!todo.empty())
		{
			entt::entity enttIDSrc = todo.front();
			todo.pop();

			_mapping[enttIDSrc] = CopyEntity(enttIDSrc);
		}

		// apply hierarchy from mapping
		for (const auto [src, dst] : _mapping)
		{
			const auto &graphNodeSrc = regSrc.get<Component_GraphNode>(src);
			auto &graphNodeDst = regDst.get<Component_GraphNode>(dst);
			if (dst != parentDst)
			{
				if (graphNodeSrc.parent != entt::null)
					graphNodeDst.parent = _mapping[graphNodeSrc.parent];
			}
			if (graphNodeSrc.firstChild != entt::null)
				graphNodeDst.firstChild = _mapping[graphNodeSrc.firstChild];
			if (graphNodeSrc.prevSibling != entt::null)
				graphNodeDst.prevSibling = _mapping[graphNodeSrc.prevSibling];
			if (graphNodeSrc.nextSibling != entt::null)
				graphNodeDst.nextSibling = _mapping[graphNodeSrc.nextSibling];
		}

		return parentDst;
	}
}
