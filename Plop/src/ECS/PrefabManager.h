#pragma once

#include <ECS/Entity.h>

namespace Plop
{
	struct Prefab
	{
		GUID guid;
		entt::entity rootId;

		Prefab(entt::entity _enttID) : rootId(_enttID) {}
	};

	struct PrefabLibrary
	{
		std::vector<Prefab> vecPrefabs;
		StringPath sPath;
		entt::registry registry;

		PrefabLibrary(const StringPath &_sPath) : sPath(_sPath) {}
	};

	class EditorLayer;
	class PrefabManager
	{
		friend class EditorLayer;

		static std::unordered_map<String, PrefabLibrary> s_mapPrefabLibs;

	public:
		static bool					CreatePrefab(Entity _entity, const String &_sLibName);
		static void					DeletePrefab(const Prefab *_pPrefab);
		static void					InstantiatePrefab(Prefab* _pPrefab, entt::registry &_reg, entt::entity _parent);

		static bool					CreatePrefabLibrary(const String &_sName, const StringPath &_sPath);
		static bool					DoesPrefabLibExist(const String &_sName);
		static void					ImGuiRenderLibraries();
		static void					LoadPrefabLibrary(const StringPath &_sPath);

		template<typename Visitor>
		static void					VisitAllLibraries(Visitor &&_v)
		{
			for (const auto &[name, lib] : s_mapPrefabLibs)
			{
				if (_v(name, lib) == VisitorFlow::BREAK)
					break;
			}
		}

	private:
		static void					SaveLibraries();
	};
}

