#include "Config.h"

#include "ECSHelper.h"

#include "ECS/Components/BaseComponents.h"

#include <entt/entt.hpp>
#include <queue>
using namespace entt::literals;

namespace Plop
{
	entt::entity CopyEntityHierarchyToRegistry(entt::handle _hSrc, entt::registry &_regDst, GUIDMapping &_guidMapping, entt::entity _rootDst /*= entt::null*/)
	{
		if (!_hSrc)
			return entt::null;

		_guidMapping.clear();
		EntityMapping entityMapping;

		entt::registry &regSrc = *_hSrc.registry();

		std::queue<entt::entity> todo;

		auto CopyEntity = [&todo, &regSrc, &_regDst](entt::entity enttIDSrc) -> entt::entity {

			entt::entity enttIDDst = _regDst.create();

			// Copy components
			// @nocheckin test
			for (auto [id, storage] : regSrc.storage())
			{
				if (storage.contains(enttIDSrc))
				{
					const entt::meta_type type = entt::resolve(id);
					type.func("clone"_hs).invoke({}, std::ref(regSrc), enttIDSrc, std::ref(_regDst), enttIDDst);
				}
			}
			/*
			for(regSrc.visit([&regSrc, &_regDst, enttIDSrc, enttIDDst](const auto compId) {
				const entt::meta_type type = entt::resolve(compId);
				type.func("clone"_hs).invoke({}, std::ref(regSrc), enttIDSrc, std::ref(_regDst), enttIDDst);
			});
			*/

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
		entt::handle parentDst { _regDst, _rootDst };
		if (!parentDst)
			parentDst = entt::handle { _regDst, CopyEntity(_hSrc.entity()) };
		else
		{
			// @nocheckin test
			for (auto [id, storage] : regSrc.storage())
			{
				const entt::meta_type type = entt::resolve(id);
				type.func("clone"_hs).invoke({}, std::ref(regSrc), _hSrc.entity(), std::ref(_regDst), parentDst);
			}
			//regSrc.visit([&regSrc, &_regDst, enttIDSrc = _hSrc.entity(), enttIDDst=parentDst](const auto compId) {
			//	const entt::meta_type type = entt::resolve(compId);
			//	type.func("clone"_hs).invoke({}, std::ref(regSrc), enttIDSrc, std::ref(_regDst), enttIDDst);
			//});

			const auto & graphNodeSrc = _hSrc.get<Component_GraphNode>();
			entt::entity childEntity = graphNodeSrc.firstChild;
			while (childEntity != entt::null)
			{
				todo.push(childEntity);

				childEntity = regSrc.get<Component_GraphNode>(childEntity).nextSibling;
			}
		}
		entityMapping[_hSrc.entity()] = parentDst;
		_guidMapping[_hSrc.get<Component_Name>().guid] = parentDst.get<Component_Name>().guid;

		// then all the rest
		while (!todo.empty())
		{
			entt::entity enttIDSrc = todo.front();
			entt::entity enttIDDst = CopyEntity(enttIDSrc);

			entityMapping[enttIDSrc]								 = enttIDDst; 
			_guidMapping[regSrc.get<Component_Name>(enttIDSrc).guid] = _regDst.get<Component_Name>(enttIDDst).guid;

			todo.pop();
		}

		// apply hierarchy from mapping
		for (const auto [src, dst] : entityMapping)
		{
			const auto &graphNodeSrc = regSrc.get<Component_GraphNode>(src);
			auto &graphNodeDst = _regDst.get<Component_GraphNode>(dst);
			if (dst != parentDst)
			{
				if (graphNodeSrc.parent != entt::null)
					graphNodeDst.parent = entityMapping[graphNodeSrc.parent];
			}
			if (graphNodeSrc.firstChild != entt::null)
				graphNodeDst.firstChild = entityMapping[graphNodeSrc.firstChild];
			if (graphNodeSrc.prevSibling != entt::null)
				graphNodeDst.prevSibling = entityMapping[graphNodeSrc.prevSibling];
			if (graphNodeSrc.nextSibling != entt::null)
				graphNodeDst.nextSibling = entityMapping[graphNodeSrc.nextSibling];
		}

		return parentDst;
	}
}
