#pragma once

#include "ECS/ECSHelper.h"
#include "ECS/Entity.h"

namespace Plop
{
	struct Prefab
	{
		GUID		 guid;
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
		StringPath			sPath;
		entt::registry		registry;
		std::unordered_map<GUID, entt::entity> mapGUIDToEntt;

		PrefabLibrary(const StringPath &_sPath) : sPath(_sPath) {}

		Entity GetEntityFromGUID(const GUID &_guid);
	};

	class EditorLayer;
	class PrefabManager
	{
		friend class EditorLayer;

	public:
		static bool			CreatePrefab(Entity _entity, const String &_sLibName);
		static void			UpdatePrefabFromInstance(PrefabHandle _hPrefab, Entity _entity);
		static void			DeletePrefab(const Prefab *_pPrefab);
		static void			LoadPrefabInstance(PrefabHandle _hPrefab, Entity _entityDst, const json &_jPatch);
		static Entity		InstantiatePrefab(PrefabHandle _hPrefab, entt::registry &_reg, entt::entity _parent);
		static Entity		InstantiatePrefab(GUID _guid, entt::registry &_reg, entt::entity _parent);
		static bool			DoesPrefabExist(GUID _guid);
		static String		GetPrefabName(GUID _guid);
		static bool			IsPartOfPrefab(Entity _entity);
		static PrefabHandle GetParentPrefab(Entity _entity);
		static json			GetChangesFromPrefab(PrefabHandle _hPrefab, const GUIDMapping &_mapping, Entity _instance);

		static bool CreatePrefabLibrary(const String &_sName, const StringPath &_sPath);
		static void	DeletePrefabLib(const PrefabLibrary *_pPrefabLib);
		static bool DoesPrefabLibExist(const String &_sName);
		static bool HasAnyPrefabLib();
		static void ImGuiRenderLibraries();
		static void LoadPrefabLibrary(const StringPath &_sPath);

		template<typename Visitor>
		static void VisitAllLibraries(Visitor &&_v)
		{
			for (const auto &[name, lib] : s_mapPrefabLibs)
			{
				if (_v(name, lib) == VisitorFlow::BREAK)
					break;
			}
		}

	private:
		static void	  SaveLibraries();
		static Entity InstantiatePrefab(PrefabLibrary &_lib, Prefab &_prefab, entt::registry &_reg, entt::entity _parent);
		static void	  UpdateInstancesFromPrefab(PrefabLibrary &_lib, Prefab &_prefab);
		static void	  RemovePrefabReferenceFromInstance(Entity _entity);

		static std::unordered_map<String, PrefabLibrary> s_mapPrefabLibs;
	};
} // namespace Plop

namespace ImGui::Custom
{
	bool InputPrefab(const char *_pLabel, Plop::PrefabHandle &_hPrefab, const ImVec2 &size_arg = ImVec2(0, 0));
}
