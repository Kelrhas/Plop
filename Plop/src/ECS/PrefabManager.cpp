#include "Plop_pch.h"

#include "PrefabManager.h"

#include "Application.h"
#include "ECS/ComponentManager.h"
#include "ECS/Components/BaseComponents.h"
#include "ECS/Components/Component_Transform.h"
#include "ECS/ECSHelper.h"
#include "ECS/Serialisation.h"
#include "Events/EntityEvent.h"
#include "Events/EventDispatcher.h"
#include "Utils/JsonTypes.h"
#include "Utils/OSDialogs.h"

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

	std::unordered_map<String, PrefabLibrary> PrefabManager::s_mapPrefabLibs;


	bool PrefabManager::CreatePrefab(Entity _entitySrc, const String &_sLibName)
	{
		if (!DoesPrefabLibExist(_sLibName))
			return false;


		PrefabLibrary &library = s_mapPrefabLibs.at(_sLibName);
		auto		   rootId  = CopyEntityHierarchyToRegistry(_entitySrc, library.registry);

		library.vecPrefabs.emplace_back(rootId);

		return true;
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

	void PrefabManager::DeletePrefab(const Prefab *_pPrefab)
	{
		for (auto &[sKey, lib] : s_mapPrefabLibs)
		{
			auto it = std::find_if(lib.vecPrefabs.begin(), lib.vecPrefabs.end(), [_pPrefab](const Prefab &_p) { return _p.guid == _pPrefab->guid; });
			if (it != lib.vecPrefabs.end())
			{
				// TODO something with existing instances ?
				lib.vecPrefabs.erase(it);
				break;
			}
		}
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

	/*Entity PrefabManager::InstantiatePrefab(Prefab *_pPrefab, entt::registry &_reg, entt::entity _parent)
	{
		ASSERT(_pPrefab);
		for (auto &[sKey, lib] : s_mapPrefabLibs)
		{
			auto it = std::find_if(lib.vecPrefabs.begin(), lib.vecPrefabs.end(), [_pPrefab](const Prefab &_p) { return _p.guid == _pPrefab->guid; });
			if (it != lib.vecPrefabs.end())
			{
				return InstantiatePrefab(lib, *it, _reg, _parent);
			}
		}

		return Entity();
	}*/

	bool PrefabManager::CreatePrefabLibrary(const String &_sName, const StringPath &_sPath)
	{
		if (DoesPrefabLibExist(_sName))
			return false;

		s_mapPrefabLibs.emplace(_sName, _sPath);

		return true;
	}

	bool PrefabManager::DoesPrefabLibExist(const String &_sName) { return s_mapPrefabLibs.find(_sName) != s_mapPrefabLibs.end(); }

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
				ImGui::InputText("###path", sNewLibPath);
				ImGui::SameLine();
				if (ImGui::Button("Browse"))
				{
					Dialog::SaveFile(sNewLibPath, Dialog::PREFABLIB_FILTER);
					sNewLibPath += ".prefablib";
				}

				if (ImGui::Button("OK"))
				{
					const String sName = sNewLibPath.stem().string();
					CreatePrefabLibrary(sName, sNewLibPath);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginChild("PrefabLibs"))
			{
				static const Prefab *pPrefabToDelete = nullptr;

				for (const auto &[sKey, lib] : s_mapPrefabLibs)
				{
					if (ImGui::CollapsingHeader(sKey.c_str()))
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
						ASSERTM(mapping.find(guid) == mapping.end(), "There already is a mapping with this guid {}", guid);
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

				Plop::Log::Info("Prefab library loaded: {} : {}", sName, _sPath.string());
			}
			else
				Plop::Log::Error("Prefab library empty: {}", _sPath.string());
		}
		else
			Plop::Log::Error("Prefab library not found: {}", _sPath.string());
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
		Log::Info("Instantiating prefab {}", nameComp.sName);

		auto newId = CopyEntityHierarchyToRegistry(entt::handle(_lib.registry, _prefab.rootId), _reg);

		Entity child(newId, _reg);

		if (_parent != entt::null)
		{
			Entity parent(_parent, _reg);
			child.SetParent(parent);
		}

		EventDispatcher::SendEvent(PrefabInstantiatedEvent(child));

		return child;
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
