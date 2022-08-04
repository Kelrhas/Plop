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

	struct PrefabHandle
	{
		GUID guid;

		constexpr PrefabHandle() : guid(GUID::INVALID) {}
		constexpr PrefabHandle(GUID _guid) : guid(_guid) {}
		void Reset() { guid = GUID::INVALID; }
		operator bool() const;
		operator GUID() const;
		bool operator==(GUID _guid) const { return _guid == guid; }
		bool operator!=(GUID _guid) const { return !(*this == _guid); }
	};
	static_assert(std::is_trivially_copyable_v<PrefabHandle>);

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
		static bool					DoesPrefabExist(GUID _guid);
		static String				GetPrefabName(GUID _guid);
		static void					DeletePrefab(const Prefab *_pPrefab);
		static Entity				InstantiatePrefab(GUID _guid, entt::registry &_reg, entt::entity _parent);
		//static Entity				InstantiatePrefab(Prefab* _pPrefab, entt::registry &_reg, entt::entity _parent);

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
		static Entity				InstantiatePrefab(PrefabLibrary &_lib, Prefab& _prefab, entt::registry &_reg, entt::entity _parent);
	};
}

namespace ImGui::Custom
{
	bool InputPrefab(const char *_pLabel, Plop::PrefabHandle &_hPrefab, const ImVec2 &size_arg = ImVec2(0, 0));
}

