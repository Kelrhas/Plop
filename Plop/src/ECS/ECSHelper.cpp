#include "Plop_pch.h"
#include "ECSHelper.h"

#include <queue>
#include <entt/entt.hpp>

#include "ECS/Components/BaseComponents.h"

namespace Plop
{
	entt::entity CopyEntityHierarchyToRegistry(entt::handle _hSrc, entt::registry &regDst)
	{
		if (!_hSrc)
			return entt::null;


		auto &regSrc = _hSrc.registry();

		std::queue<entt::entity> todo;

		// mapping from src to dst
		std::unordered_map<entt::entity, entt::entity> mapping;

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
		entt::entity parentDst = CopyEntity(_hSrc.entity());
		mapping[_hSrc.entity()] = parentDst;

		// then all the rest
		while (!todo.empty())
		{
			entt::entity enttIDSrc = todo.front();
			todo.pop();

			mapping[enttIDSrc] = CopyEntity(enttIDSrc);
		}

		// apply hierarchy from mapping
		for (const auto [src, dst] : mapping)
		{
			const auto &graphNodeSrc = regSrc.get<Component_GraphNode>(src);
			auto &graphNodeDst = regDst.get<Component_GraphNode>(dst);
			if (dst != parentDst)
			{
				if (graphNodeSrc.parent != entt::null)
					graphNodeDst.parent = mapping[graphNodeSrc.parent];
			}
			if (graphNodeSrc.firstChild != entt::null)
				graphNodeDst.firstChild = mapping[graphNodeSrc.firstChild];
			if (graphNodeSrc.prevSibling != entt::null)
				graphNodeDst.prevSibling = mapping[graphNodeSrc.prevSibling];
			if (graphNodeSrc.nextSibling != entt::null)
				graphNodeDst.nextSibling = mapping[graphNodeSrc.nextSibling];
		}

		return parentDst;
	}
}
