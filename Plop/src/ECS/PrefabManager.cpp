#include "Plop_pch.h"

#include "PrefabManager.h"

#include "Application.h"
#include "ECS/ComponentManager.h"
#include "ECS/Components/BaseComponents.h"
#include "ECS/Components/Component_PrefabInstance.h"
#include "ECS/Components/Component_Transform.h"
#include "ECS/Serialisation.h"
#include "Events/EntityEvent.h"
#include "Events/EventDispatcher.h"
#include "Utils/JsonTypes.h"
#include "Utils/OSDialogs.h"

#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122
#include <entt/entt.hpp>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
	#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <queue>

namespace Plop
{
	/*
	*
Utiliser EntityMgr avec un registre pour y stocker les prefabs
- remanier la lecture des fichiers sources pour pouvoir charger des prefabs et des instances avec uniquement les valeurs changées
- interdire prefab de prefab pour le moment ?
- stocker entt::entity du prefab dans Entity ou GUID ?
- stocker liste des entt::entity parents de prefab
- stocker instance du prefab avec GUID d'un level + GUID de l'entity instance


usage:
- créer un prefab à partir d'une entity dans la scène
	-> créer prefab
	-> lié entity à un prefab

- instancier un prefab
	-> réagir à un changement dans le prefab
	-> override certaines valeurs
- supprimer lien avec un prefab
	-> pas grand chose en vrai pour ça
	*/

	PrefabHandle::operator bool() const
	{
		if (guid == GUID::INVALID)
			return false;

		return PrefabManager::DoesPrefabExist(guid);
	}

	PrefabHandle::operator GUID() const { return guid; }

	Entity PrefabLibrary::GetEntityFromGUID(const GUID &_guid)
	{
		auto it = mapGUIDToEntt.find(_guid);
		if(it != mapGUIDToEntt.end())
			return Entity(it->second, registry);

		return Entity{};
	}


	std::unordered_map<String, PrefabLibrary> PrefabManager::s_mapPrefabLibs;


	bool PrefabManager::CreatePrefab(Entity _entitySrc, const String &_sLibName)
	{
		if (!DoesPrefabLibExist(_sLibName))
			return false;

		PrefabLibrary &library = s_mapPrefabLibs.at(_sLibName);
		GUIDMapping	   mapping;
		auto		   rootId = CopyEntityHierarchyToRegistry(_entitySrc, library.registry, mapping);

		library.vecPrefabs.emplace_back(rootId);

		Entity entity(rootId, library.registry);
		entity.VisitChildrenRecursive([&library](Entity _entity)
		{
		    auto& comp = _entity.GetComponent<Component_Name>();
			library.mapGUIDToEntt[comp.guid] = _entity;
			return VisitorFlow::CONTINUE;
		});

		return true;
	}

	void PrefabManager::UpdatePrefabFromInstance(PrefabHandle _hPrefab, Entity _entity)
	{
		ASSERT(_hPrefab);
		ASSERT(_entity);

		for (auto &[sKey, lib] : s_mapPrefabLibs)
		{
			auto it = std::find_if(lib.vecPrefabs.begin(), lib.vecPrefabs.end(), [_hPrefab](const Prefab &_p) { return _p.guid == _hPrefab.guid; });
			if (it != lib.vecPrefabs.end())
			{
				auto &		compInstance = _entity.GetComponent<Component_PrefabInstance>();
				auto &		regPrefab	 = lib.registry;
				const auto &regLevel	 = entt::handle(_entity).registry();
				LevelBasePtr xLevel		 = Application::GetCurrentLevel().lock();

				for (auto &[prefabEntityGUID, levelEntityGUID] : compInstance.mapping)
				{
					Entity prefabEntity = lib.GetEntityFromGUID(prefabEntityGUID);
					Entity levelEntity	= xLevel->GetEntityFromGUID(levelEntityGUID);
					// add/remove component in prefab if necessary
					ComponentManager::VisitAllComponents(
					  [&](const entt::id_type _compId, const ComponentManager::ComponentInfo &_compInfo)
					  {
						  if (_compId == entt::type_info<Component_Name>::id())
							  return;
						  if (_compId == entt::type_info<Component_GraphNode>::id())
							  return;
						  if (_compId == entt::type_info<Component_PrefabInstance>::id())
							  return;

						const bool prefabHas = ComponentManager::HasComponent(regPrefab, prefabEntity, _compId);
						const bool levelHas	 = ComponentManager::HasComponent(regLevel, levelEntity, _compId);
				        
						if(levelHas)
						{
							_compInfo.funcDuplicate(regLevel, levelEntity, regPrefab, prefabEntity);
						}
						else if(prefabHas)
						{
							_compInfo.funcRemove(regPrefab, prefabEntity);
						}
					}); 
				}

				UpdateInstancesFromPrefab(lib, *it);

				return;
			}
		}

		ASSERTM(false, "Prefab not found for entity {}", _entity.GetComponent<Component_Name>().sName);
	}

	void PrefabManager::DeletePrefab(const Prefab *_pPrefab)
	{
		for (auto &[sKey, lib] : s_mapPrefabLibs)
		{
			auto it = std::find_if(lib.vecPrefabs.begin(), lib.vecPrefabs.end(), [_pPrefab](const Prefab &_p) { return _p.guid == _pPrefab->guid; });
			if (it != lib.vecPrefabs.end())
			{
				auto &regLevel = Application::GetCurrentLevel().lock()->GetEntityRegistry();
				regLevel.view<Component_PrefabInstance>().each([_pPrefab, &regLevel](entt::entity _enttID, const Component_PrefabInstance &_instanceComp) {
					if (_instanceComp.hSrcPrefab.guid == _pPrefab->guid)
					{
						RemovePrefabReferenceFromInstance(Entity(_enttID, regLevel));
					}
				});

				lib.vecPrefabs.erase(it);
				break;
			}
		}
	}

	void PrefabManager::LoadPrefabInstance(PrefabHandle _hPrefab, Entity _entityDst, const json &_jPatch)
	{
		ASSERTM(_entityDst.HasComponent<Component_PrefabInstance>(), "Add Component_PrefabInstance to the entity first");
		if (!_entityDst.HasComponent<Component_PrefabInstance>())
			return;

		for (auto &[sKey, lib] : s_mapPrefabLibs)
		{
			auto it = std::find_if(lib.vecPrefabs.begin(), lib.vecPrefabs.end(), [_hPrefab](const Prefab &_p) { return _p.guid == _hPrefab.guid; });
			if (it != lib.vecPrefabs.end())
			{
				GUIDMapping mapping;
				CopyEntityHierarchyToRegistry(entt::handle(lib.registry, it->rootId), entt::handle(_entityDst).registry(), mapping, _entityDst);
				
				auto &prefabInstanceComp		   = _entityDst.GetComponent<Component_PrefabInstance>();
				prefabInstanceComp.hSrcPrefab.guid = _hPrefab.guid;
				prefabInstanceComp.mapping		   = std::move(mapping);

				std::function<void(Entity, Entity)> func;

				func = [&_jPatch, &prefabInstanceComp, &func](Entity _prefabEntity, Entity _instanceEntity) {
					{ // RAII
						json jPrefab;
						ComponentManager::ToJson(entt::handle(_prefabEntity).registry(), _prefabEntity, jPrefab);
						json jInstance;
						ComponentManager::ToJson(entt::handle(_instanceEntity).registry(), _instanceEntity, jInstance);

						String sKey = fmt::format("{}", _prefabEntity.GetComponent<Component_Name>().guid);
						if (_jPatch.contains(sKey))
							jPrefab = jPrefab.patch(_jPatch[sKey]);

						_instanceEntity.FromJson(jPrefab);
					}

					
					// children, iterate over prefab, and match the instance from the mapping
					_prefabEntity.VisitChildren([&prefabInstanceComp, _instanceEntity, &func](Entity childPrefab) {
						const GUID guidPrefab	= childPrefab.GetComponent<Component_Name>().guid;
						const GUID guidInstance = prefabInstanceComp.mapping.at(guidPrefab);
						_instanceEntity.VisitChildren([&](Entity _childInstance) {
							if (_childInstance.GetComponent<Component_Name>().guid == guidInstance)
							{
								func(childPrefab, _childInstance);
								return VisitorFlow::BREAK;
							}
							return VisitorFlow::CONTINUE;
						});

						return VisitorFlow::CONTINUE;
					});
				};

				Entity prefabEntity(it->rootId, lib.registry);
				func(prefabEntity, _entityDst);

				EventDispatcher::SendEvent(PrefabInstantiatedEvent(_entityDst));

				return;
			}
		}
		Plop::Log::Error("Prefab not found with the GUID {}", _hPrefab.guid);
	}

	Entity PrefabManager::InstantiatePrefab(PrefabHandle _hPrefab, entt::registry &_reg, entt::entity _parent)
	{
		return InstantiatePrefab(_hPrefab.guid, _reg, _parent);
	}

	Entity PrefabManager::InstantiatePrefab(GUID _guid, entt::registry &_reg, entt::entity _parent)
	{
		for (auto &[sKey, lib] : s_mapPrefabLibs)
		{
			auto it = std::find_if(lib.vecPrefabs.begin(), lib.vecPrefabs.end(), [_guid](const Prefab &_p) { return _p.guid == _guid; });
			if (it != lib.vecPrefabs.end())
			{
				return InstantiatePrefab(lib, *it, _reg, _parent);
			}
		}

		Plop::Log::Error("Prefab not found with the GUID {}", _guid);
		return Entity();
	}

	bool PrefabManager::DoesPrefabExist(GUID _guid)
	{
		for (auto &[sKey, lib] : s_mapPrefabLibs)
		{
			auto it = std::find_if(lib.vecPrefabs.begin(), lib.vecPrefabs.end(), [_guid](const Prefab &_p) { return _p.guid == _guid; });
			if (it != lib.vecPrefabs.end())
			{
				return true;
			}
		}

		return false;
	}

	String PrefabManager::GetPrefabName(GUID _guid)
	{
		for (auto &[sKey, lib] : s_mapPrefabLibs)
		{
			auto it = std::find_if(lib.vecPrefabs.begin(), lib.vecPrefabs.end(), [_guid](const Prefab &_p) { return _p.guid == _guid; });
			if (it != lib.vecPrefabs.end())
			{
				const auto &nameComp = lib.registry.get<Component_Name>(it->rootId);
				return nameComp.sName;
			}
		}

		ASSERTM(false, "There is no prefab with GUID {}", _guid);
		return "";
	}

	bool PrefabManager::IsPartOfPrefab(Entity _entity)
	{
		while (_entity)
		{
			if (_entity.HasComponent<Component_PrefabInstance>())
				return true;

			_entity = _entity.GetParent();
		}

		return false;
	}

	PrefabHandle PrefabManager::GetParentPrefab(Entity _entity)
	{
		while (_entity)
		{
			if (_entity.HasComponent<Component_PrefabInstance>())
			{
				const auto &prefabComp = _entity.GetComponent<Component_PrefabInstance>();
				return prefabComp.hSrcPrefab;
			}

			const auto graphComp = _entity.GetComponent<Component_GraphNode>();
			_entity				 = _entity.GetParent();
		}

		return PrefabHandle();
	}

	json PrefabManager::GetChangesFromPrefab(PrefabHandle _hPrefab, const GUIDMapping &_mapping, Entity _instance)
	{
		json j;

		for (auto &[sKey, lib] : s_mapPrefabLibs)
		{
			auto it = std::find_if(lib.vecPrefabs.begin(), lib.vecPrefabs.end(), [_hPrefab](const Prefab &_p) { return _p.guid == _hPrefab.guid; });
			if (it != lib.vecPrefabs.end())
			{
				Entity		prefabEntity(it->rootId, lib.registry);
				Entity		instanceEntity = _instance;
				const auto &prefabInstanceComp = _instance.GetComponent<Component_PrefabInstance>();

				std::function<void(Entity, Entity)> func;
				func = [&j, &prefabInstanceComp, &func](Entity _prefabEntity, Entity _instanceEntity) {
					{ // RAII
						json jPrefab;
						ComponentManager::ToJson(entt::handle(_prefabEntity).registry(), _prefabEntity, jPrefab);
						json jInstance;
						ComponentManager::ToJson(entt::handle(_instanceEntity).registry(), _instanceEntity, jInstance);

						json patch														= json::diff(jPrefab, jInstance);
						j[fmt::format("{}", _prefabEntity.GetComponent<Component_Name>().guid)] = patch;
					}

					// children, iterate over prefab, and match the instance from the mapping
					_prefabEntity.VisitChildren([&prefabInstanceComp, _instanceEntity, &func](Entity childPrefab) {
						const GUID guidPrefab	= childPrefab.GetComponent<Component_Name>().guid;
						const GUID guidInstance = prefabInstanceComp.mapping.at(guidPrefab);

						_instanceEntity.VisitChildren([&](Entity _childInstance) {
							if (_childInstance.GetComponent<Component_Name>().guid == guidInstance)
							{
								func(childPrefab, _childInstance);
								return VisitorFlow::BREAK;
							}
							return VisitorFlow::CONTINUE;
						});

						return VisitorFlow::CONTINUE;
					});
				};

				func(prefabEntity, instanceEntity);

				break;
			}
		}

		return j;

	}

	bool PrefabManager::CreatePrefabLibrary(const String &_sName, const StringPath &_sPath)
	{
		if (DoesPrefabLibExist(_sName))
			return false;

		s_mapPrefabLibs.emplace(_sName, _sPath);

		return true;
	}

	void PrefabManager::DeletePrefabLib(const PrefabLibrary *_pPrefabLib)
	{
		ASSERT(_pPrefabLib != nullptr);
		for (auto it = s_mapPrefabLibs.begin(); it != s_mapPrefabLibs.end(); ++it)
		{
			if (&it->second == _pPrefabLib)
			{
				s_mapPrefabLibs.erase(it);
				break;
			}
		}
	}

	bool PrefabManager::DoesPrefabLibExist(const String &_sName) { return s_mapPrefabLibs.find(_sName) != s_mapPrefabLibs.end(); }

	bool PrefabManager::HasAnyPrefabLib() { return !s_mapPrefabLibs.empty(); }

	void PrefabManager::ImGuiRenderLibraries()
	{
		if (ImGui::Begin("Prefab libraries"))
		{
			static StringPath sNewLibPath;
			if (ImGui::Button("New library"))
			{
				ImGui::OpenPopup("NewPrefabLib");
				sNewLibPath.clear();
			}

			if (ImGui::BeginPopup("NewPrefabLib"))
			{
				ImGui::InputText("###path", sNewLibPath, ImGuiInputTextFlags_ReadOnly);
				ImGui::SameLine();
				if (ImGui::Button("Browse"))
				{
					if (Dialog::SaveFile(sNewLibPath, Dialog::PREFABLIB_FILTER))
					{
						if (!sNewLibPath.has_extension() || sNewLibPath.extension() != ".prefablib")
							sNewLibPath += ".prefablib";
					}
				}

				ImGuiButtonFlags buttonFlags = ImGuiButtonFlags_None;
				if (sNewLibPath.empty())
					buttonFlags |= ImGuiButtonFlags_Disabled;
				if (ImGui::ButtonEx("OK", ImVec2(0, 0), buttonFlags))
				{
					const String sName = sNewLibPath.stem().string();
					CreatePrefabLibrary(sName, sNewLibPath);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Open library"))
			{
				if (Dialog::OpenFile(sNewLibPath, Dialog::PREFABLIB_FILTER))
				{
					LoadPrefabLibrary(sNewLibPath);
				}
			}

			if (ImGui::BeginChild("PrefabLibs"))
			{
				static const PrefabLibrary *pPrefabLibToDelete = nullptr;
				static const Prefab *pPrefabToDelete = nullptr;

				for (const auto &[sKey, lib] : s_mapPrefabLibs)
				{
					bool bOpen = true;
					bool *pOpenBool = (lib.vecPrefabs.empty() ? &bOpen : nullptr);
					if (ImGui::CollapsingHeader(sKey.c_str(), pOpenBool))
					{
						for (const auto &prefab : lib.vecPrefabs)
						{
							if (lib.registry.has<Component_Name>(prefab.rootId))
							{
								const auto &comp = lib.registry.get<Component_Name>(prefab.rootId);

								ImGui::PushID((int)(uint64_t)comp.guid);
								ImGui::Indent();

								ImGui::Selectable(comp.sName.c_str(), false);


								if (ImGui::BeginPopupContextItem("EntityContextMenu"))
								{
									if (ImGui::MenuItem("Delete prefab"))
										pPrefabToDelete = &prefab;
									if (ImGui::MenuItem("Copy GUID"))
									{
										String s = fmt::format("{}", prefab.guid);
										ImGui::SetClipboardText(s.c_str());
									}

									ImGui::EndPopup();
								}
								else
								{
									if (ImGui::BeginDragDropSource())
									{
										ImGui::SetDragDropPayload("InstantiatePrefab", &prefab.guid, sizeof(prefab.guid));

										ImGui::Text(comp.sName.c_str());
										ImGui::EndDragDropSource();
									}
								}

								ImGui::Unindent();
								ImGui::PopID();
							}
							else
								ImGui::Text("no comp Name for %d", prefab.rootId);
						}
					}

					if (pOpenBool && *pOpenBool == false)
					{
						ASSERT(lib.vecPrefabs.empty());
						pPrefabLibToDelete = &lib;
					}
				}

				if (pPrefabLibToDelete)
				{
					DeletePrefabLib(pPrefabLibToDelete);
				}

				if (pPrefabToDelete && !ImGui::IsPopupOpen("DeletePrefab"))
				{
					ImGui::OpenPopup("DeletePrefab");
				}

				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
				if (ImGui::BeginPopupModal("DeletePrefab", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ASSERT(pPrefabToDelete);
					ImGui::Text("Are you sure you want to delete this prefab ?");

					auto & regLevel	  = Application::GetCurrentLevel().lock()->GetEntityRegistry();
					size_t nbInstance = 0;
					regLevel.view<Component_PrefabInstance>().each([&nbInstance](const Component_PrefabInstance &_instanceComp) {
						if (_instanceComp.hSrcPrefab.guid == pPrefabToDelete->guid)
							++nbInstance;
					});

					if (nbInstance > 0)
						ImGui::Text("Currently used by %d instances", nbInstance);

					if (ImGui::Button("Yes"))
					{
						DeletePrefab(pPrefabToDelete);
						pPrefabToDelete = nullptr;
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					if (ImGui::Button("No"))
					{
						pPrefabToDelete = nullptr;
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

	void PrefabManager::LoadPrefabLibrary(const StringPath &_sPath)
	{
		bool bAlreadyLoaded = false;
		VisitAllLibraries([&](const String &, const PrefabLibrary &lib) {
			if (lib.sPath == _sPath)
			{
				bAlreadyLoaded = true;
				return VisitorFlow::BREAK;
			}

			return VisitorFlow::CONTINUE;
		});

		if (bAlreadyLoaded)
			return;


		// std::filesystem::create_directories(_sPath.parent_path());
		std::ifstream libraryFile(_sPath, std::ios::in);

		if (libraryFile.is_open())
		{
			json jLib;
			libraryFile >> jLib;

			if (!jLib.empty())
			{
				String sName = jLib[JSON_NAME];
				CreatePrefabLibrary(sName, _sPath);
				PrefabLibrary &lib = s_mapPrefabLibs.at(sName);

				std::unordered_map<GUID, entt::entity> mapping;
				if (jLib.contains(JSON_ENTITIES))
				{
					// create entities
					for (const auto &j : jLib[JSON_ENTITIES])
					{
						entt::entity enttID = lib.registry.create();
						GUID		 guid	= j[JSON_GUID];
						lib.registry.emplace<Component_Name>(enttID, guid);
						ASSERTM(mapping.find(guid) == mapping.end(), "There already is a mapping with the guid {}", guid);
						mapping[guid] = enttID;
						lib.registry.emplace<Component_GraphNode>(enttID);
						lib.registry.emplace<Component_Transform>(enttID);
					}

					// and apply components & hierarchy
					for (const auto &j : jLib[JSON_ENTITIES])
					{
						GUID		 guid							   = j[JSON_GUID];
						entt::entity enttID							   = mapping[guid];
						lib.registry.get<Component_Name>(enttID).sName = j[JSON_NAME];

						if (j.contains(JSON_CHILDREN))
						{
							auto &		 graphComp		 = lib.registry.get<Component_GraphNode>(enttID);
							entt::entity enttIDLastChild = entt::null;
							for (const auto &jChild : j[JSON_CHILDREN])
							{
								GUID		 guidChild		= jChild;
								entt::entity enttIDChild	= mapping[guidChild];
								auto &		 graphCompChild = lib.registry.get<Component_GraphNode>(enttIDChild);
								graphCompChild.parent		= enttID;
								if (enttIDLastChild == entt::null)
								{
									graphComp.firstChild = enttIDChild;
								}
								else
								{
									auto &graphCompLastChild	   = lib.registry.get<Component_GraphNode>(enttIDLastChild);
									graphCompChild.prevSibling	   = enttIDLastChild;
									graphCompLastChild.nextSibling = enttIDChild;
								}
								enttIDLastChild = enttIDChild;
							}
						}

						if (j.contains(JSON_COMPONENTS))
						{
							ComponentManager::FromJson(lib.registry, enttID, j[JSON_COMPONENTS]);
						}
					}
				}

				if (jLib.contains(JSON_PREFABS))
				{
					for (auto &jPrefab : jLib[JSON_PREFABS])
					{
						ASSERT(jPrefab.contains(JSON_ROOT));
						ASSERT(jPrefab.contains(JSON_GUID));
						GUID   guid = jPrefab[JSON_ROOT];
						Prefab prefab(mapping[guid]);
						prefab.guid = jPrefab[JSON_GUID];
						lib.vecPrefabs.emplace_back(prefab);
					}
				}

				
				lib.registry.each(
				  [&lib](entt::entity _entt)
				  {
					  auto &comp				   = lib.registry.get<Component_Name>(_entt);
					  lib.mapGUIDToEntt[comp.guid] = _entt;
					  return VisitorFlow::CONTINUE;
				  });

				Plop::Log::Info("Prefab library '{}' loaded from file {}", sName, _sPath.string());
			}
			else
				Plop::Log::Error("Prefab library '{}' empty", _sPath.string());
		}
		else
			Plop::Log::Error("Prefab library '{}' not found", _sPath.string());
	}

	void PrefabManager::SaveLibraries()
	{
		for (const auto &itPair : s_mapPrefabLibs)
		{
			auto &sKey = itPair.first;
			auto &lib  = itPair.second;

			std::filesystem::create_directories(lib.sPath.parent_path());
			std::ofstream libraryFile(lib.sPath, std::ios::out | std::ios::trunc);
			if (libraryFile.is_open())
			{
				json jLib;
				jLib[JSON_NAME] = sKey;


				lib.registry.each([&jLib, &lib](entt::entity enttID) {
					// TODO maybe merge with Entity::ToJson ?
					auto &nameComp = lib.registry.get<Component_Name>(enttID);
					json  j;
					j[JSON_NAME]			= nameComp.sName;
					j[JSON_GUID]			= nameComp.guid;
					const auto &graphNode	= lib.registry.get<Component_GraphNode>(enttID);
					auto		childEntity = graphNode.firstChild;
					while (childEntity != entt::null)
					{
						auto &nameCompChild = lib.registry.get<Component_Name>(childEntity);
						j[JSON_CHILDREN].push_back(nameCompChild.guid);

						childEntity = lib.registry.get<Component_GraphNode>(childEntity).nextSibling;
					}

					ComponentManager::ToJson(lib.registry, enttID, j);
					jLib[JSON_ENTITIES].emplace_back(std::move(j));
				});

				for (const auto &prefab : lib.vecPrefabs)
				{
					auto &nameComp	   = lib.registry.get<Component_Name>(prefab.rootId);
					json &jPrefab	   = jLib[JSON_PREFABS].emplace_back();
					jPrefab[JSON_ROOT] = nameComp.guid;
					jPrefab[JSON_NAME] = nameComp.sName;
					jPrefab[JSON_GUID] = prefab.guid;
				}

				libraryFile << jLib.dump(2);
			}
		}
	}

	Entity PrefabManager::InstantiatePrefab(PrefabLibrary &_lib, Prefab &_prefab, entt::registry &_reg, entt::entity _parent)
	{
		auto &nameComp = _lib.registry.get<Component_Name>(_prefab.rootId);
		Log::Info("Instantiating prefab '{}'", nameComp.sName);

		GUIDMapping mapping;
		auto		  newId = CopyEntityHierarchyToRegistry(entt::handle(_lib.registry, _prefab.rootId), _reg, mapping);

		Entity child(newId, _reg);

		if (_parent != entt::null)
		{
			Entity parent(_parent, _reg);
			child.SetParent(parent);
		}

		auto &prefabInstanceComp		   = child.AddComponent<Component_PrefabInstance>();
		prefabInstanceComp.hSrcPrefab.guid = _prefab.guid;
		prefabInstanceComp.mapping		   = std::move(mapping);

		EventDispatcher::SendEvent(PrefabInstantiatedEvent(child));

		return child;
	}

	void PrefabManager::UpdateInstancesFromPrefab(PrefabLibrary &_lib, Prefab &_prefab)
	{
		auto &regLevel = Application::GetCurrentLevel().lock()->GetEntityRegistry();
		auto  view	   = regLevel.view<Component_PrefabInstance>();
		view.each([&](const Component_PrefabInstance& _prefabComp) {
			if (_prefabComp.hSrcPrefab.guid == _prefab.guid)
			{
				auto &		regPrefab	 = _lib.registry;

				//for (auto &[prefabEntityGUID, levelEntityGUID] : _prefabComp.mapping)
				//{
				//	Entity prefabEntity = _lib.GetEntityFromGUID(prefabEntityGUID);
				//	Entity levelEntity	= _lib.GetEntityFromGUID(levelEntityGUID);
				//	// add/remove component in level if necessary
				//	ComponentManager::VisitAllComponents([&](const entt::id_type _compId, const ComponentManager::ComponentInfo &_compInfo) {
				//
				//		const bool prefabHas = ComponentManager::HasComponent(regPrefab, prefabEntity, _compId);
				//		const bool levelHas	 = ComponentManager::HasComponent(regLevel, levelEntity, _compId);
				//
				//		if(prefabHas)
				//		{
				//			_compInfo.funcDuplicate(regPrefab, prefabEntity, regLevel, levelEntity);
				//		}
				//		else if(levelHas)
				//		{
				//			_compInfo.funcRemove(regLevel, levelEntity);
				//		}
				//	});
				//}
			}
		});
	}

	void PrefabManager::RemovePrefabReferenceFromInstance(Entity _entity)
	{
		_entity.RemoveComponent<Component_PrefabInstance>();
	}

} // namespace Plop

#include "Assets/SpritesheetLoader.h"

namespace ImGui::Custom
{
	bool InputPrefab(const char *label, Plop::PrefabHandle &_hPrefab, const ImVec2 &size_arg)
	{
		ImGuiWindow *window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext &	  g			 = *GImGui;
		const ImGuiStyle &style		 = g.Style;
		const ImGuiID	  id		 = window->GetID(label);
		const float		  w			 = CalcItemWidth();
		const ImVec2	  label_size = CalcTextSize(label, NULL, true);
		const ImVec2	  frame_size = CalcItemSize(size_arg, CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
		const ImRect	  frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect	  total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id, &frame_bb))
			return false;

		// Draw frame
		const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		RenderNavHighlight(frame_bb, id);
		RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

		bool value_changed = false;

		if (BeginDragDropTarget())
		{
			if (const ImGuiPayload *pPayload = ImGui::AcceptDragDropPayload("InstantiatePrefab"))
			{
				ASSERTM(pPayload->DataSize == sizeof(Plop::GUID), "Wrong Drag&Drop payload");
				Plop::GUID guid = *(Plop::GUID *)pPayload->Data;
				if (guid != _hPrefab)
				{
					_hPrefab	  = guid;
					value_changed = true;
				}
			}
			EndDragDropTarget();
		}

		// icon
		auto	  hSpriteSheet = Plop::AssetLoader::GetSpritesheet(Plop::Application::Get()->GetEditorDirectory() / "assets/icons/editor.ssdef");
		glm::vec2 vMin, vMax;
		if (hSpriteSheet->GetSpriteUV("prefab", vMin, vMax))
		{
			// Image((ImTextureID)hSpriteSheet->GetNativeHandle(), ImVec2(0, 0), vMin, vMax);
			const ImRect image_bb(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + frame_bb.GetHeight(), frame_bb.Min.y + frame_bb.GetHeight());
			window->DrawList->AddImage((ImTextureID)hSpriteSheet->GetNativeHandle(), image_bb.Min, image_bb.Max, vMin, vMax);
		}


		// reset button
		ImVec2 button_size(frame_size.y, frame_size.y);
		if (_hPrefab != Plop::GUID::INVALID)
		{
			ImRect		  button_bb(frame_bb.Max.x - button_size.x, frame_bb.Min.y, frame_bb.Max.x, frame_bb.Max.y);
			const ImGuiID button_id = window->GetID("Reset");

			if (CloseButton(button_id, ImVec2(button_bb.Min.x, button_bb.Min.y)))
			// if (ButtonBehavior(button_bb, button_id, nullptr, nullptr, ImGuiButtonFlags_AlignTextBaseLine))
			{
				_hPrefab.Reset();
			}
		}

		String sName = "None";
		if (_hPrefab)
			sName = Plop::PrefabManager::GetPrefabName(_hPrefab);
		else if (_hPrefab != Plop::GUID::INVALID)
			sName = "Invalid";
		ImVec2		 draw_pos		 = frame_bb.Min + style.FramePadding + ImVec2(frame_bb.GetHeight(), 0.f);
		const char * buf_display	 = sName.c_str();
		const char * buf_display_end = buf_display + strlen(buf_display);
		ImU32		 col			 = GetColorU32(ImGuiCol_Text);
		const ImVec4 clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + frame_size.x - button_size.x, frame_bb.Min.y + frame_size.y);
		window->DrawList->AddText(g.Font, g.FontSize, draw_pos, col, buf_display, buf_display_end, 0.0f, &clip_rect);

		// label
		if (label_size.x > 0.0f)
			RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

		if (value_changed)
			MarkItemEdited(id);

		return value_changed;
	}
} // namespace ImGui::Custom
