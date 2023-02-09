#include "TD_pch.h"

#include "Component_Hexgrid.h"

#include "ECS/Components/Component_Tower.h"

#include <Assets/SpritesheetLoader.h>
#include <ECS/Components/BaseComponents.h>
#include <ECS/ECSHelper.h>
#include <ECS/PrefabManager.h>
#include <Input/Input.h>
#include <Math/Math.h>
#include <Utils/JsonTypes.h>
#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122

constexpr const char *JSON_WIDTH		 = "Width";
constexpr const char *JSON_HEIGHT		 = "Height";
constexpr const char *JSON_CELL_OVERRIDE = "Cells";
constexpr const char *JSON_SPAWNER_COORD = "SpawnerCoord";
constexpr const char *JSON_BASE_COORD	 = "BaseCoord";

namespace Private
{
	Component_Hexgrid *pEditingComp = nullptr;
	enum class PickingMode
	{
		NONE,
		CELL,
		BASE,
		SPAWNER
	} ePickingMode		= PickingMode::NONE;
	CellType ePaintType = CellType::INVALID;
} // namespace Private


void Component_Hexgrid::RegenerateGrid()
{
	Plop::LevelBasePtr xLevel = Plop::Application::GetCurrentLevel().lock();
	Plop::Entity	   owner  = Plop::GetComponentOwner(xLevel->GetEntityRegistry(), *this);

	// @check destroying inside visitor ...
	owner.VisitChildren([&](Plop::Entity _child) {
		xLevel->DestroyEntity(_child);
		return VisitorFlow::CONTINUE;
	});

	size_t nb = owner.GetChildrenCount();
	ASSERT(!owner.HasChildren());

	StringPath sSpritesheet = std::filesystem::canonical("assets/textures/hexagons.ssdef");
	auto	   hSpritesheet = Plop::AssetLoader::GetSpritesheet(sSpritesheet);

	m_grid.Init(uWidth, uHeight, owner);
	for (const auto &cellOverride : vecCells)
	{
		Hexgrid::Cell *pCell = nullptr;
		if (m_grid.GetModifiableCell(cellOverride.coord, &pCell))
		{
			Plop::Entity e = pCell->entity;
			*pCell		   = cellOverride;
			pCell->entity  = e;
			pCell->ApplyCellType();
		}
	}

	m_grid.SetBaseCoord(baseCoord);
	m_grid.SetSpawnerCoord(spawnerCoord);
}


namespace ImGui
{
	template<class Enum, typename Callback>
	void EnumDropdown(const char *_pLabel, Enum current_item, int items_count, Callback &&_callback)
	{
		json j;
		nlohmann::to_json(j, current_item);
		if (ImGui::BeginCombo(_pLabel, j.dump().c_str()))
		{
			for (int i = 0; i < items_count; ++i)
			{
				nlohmann::to_json(j, (Enum)i);
				if (ImGui::Selectable(j.dump().c_str(), (int)current_item == i))
				{
					_callback((Enum)i);
				}
			}
			ImGui::EndCombo();
		}
	}
} // namespace ImGui


void Component_Hexgrid::EditorUI()
{
	Plop::LevelBasePtr xLevel = Plop::Application::GetCurrentLevel().lock();

	U32	 uMin = 2, uMax = 100;
	bool bDirty = false;
	bDirty |= ImGui::DragScalar("Width", ImGuiDataType_U32, &uWidth, 0.1f, &uMin, &uMax);
	bDirty |= ImGui::DragScalar("Height", ImGuiDataType_U32, &uHeight, 0.1f, &uMin, &uMax);
	// TODO: check if some cell inside vecCells are being removed

	if (ImGui::Button("Generate") || bDirty)
	{
		RegenerateGrid();
	}


	ImGui::Separator();
	if (ImGui::TreeNode("Cells"))
	{
		// TODO: switch to BegintTable once imgui is up to date

		ImGui::Columns(2);
		ImGui::Text("Coord");
		ImGui::NextColumn();
		ImGui::Text("Type");
		ImGui::NextColumn();
		for (Hexgrid::Cell &cell : vecCells)
		{
			ImGui::Text("%d %d %d", cell.coord.x, cell.coord.y, cell.coord.z);
			if (ImGui::IsItemHovered())
			{
				// TODO make the cell blink
			}

			ImGui::NextColumn();

			ImGui::PushID((int)std::hash<Hexgrid::Cell>()(cell));

			ImGui::EnumDropdown("", cell.eType, (int)CellType::COUNT, [&](CellType _eNewType) {
				cell.eType = _eNewType;

				Hexgrid::Cell *pInstance = nullptr;
				m_grid.GetModifiableCell(cell.coord, &pInstance);
				pInstance->eType = _eNewType;
				pInstance->ApplyCellType();
			});

			ImGui::PopID();
			ImGui::NextColumn();
		}
		ImGui::Columns();

		ImGui::TreePop();
	}

	if (Private::ePaintType != CellType::INVALID)
	{
		ImGui::EnumDropdown("Type to paint", Private::ePaintType, (int)CellType::COUNT, [&](CellType _eNewType) { Private::ePaintType = _eNewType; });

		if (ImGui::Button("Stop"))
		{
			Private::ePaintType	  = CellType::INVALID;
			Private::pEditingComp = nullptr;
			Plop::Application::Get()->GetEditor().RestorePicking();
		}
	}
	else
	{
		if (ImGui::Button("Pick cell to override"))
		{
			Private::ePickingMode = Private::PickingMode::CELL;
			Private::pEditingComp = this;
			Plop::Application::Get()->GetEditor().PreventPicking();
		}

		if (ImGui::Button("Paint cells"))
		{
			Private::ePaintType	  = CellType::WALL;
			Private::pEditingComp = this;
			Plop::Application::Get()->GetEditor().PreventPicking();
		}
		if (ImGui::Button("Place base"))
		{
			Private::ePickingMode = Private::PickingMode::BASE;
			Private::pEditingComp = this;
			Plop::Application::Get()->GetEditor().PreventPicking();
		}
		if (ImGui::Button("Place spawner"))
		{
			Private::ePickingMode = Private::PickingMode::SPAWNER;
			Private::pEditingComp = this;
			Plop::Application::Get()->GetEditor().PreventPicking();
		}
	}
}

json Component_Hexgrid::ToJson() const
{
	json j;
	j[JSON_WIDTH]		  = uWidth;
	j[JSON_HEIGHT]		  = uHeight;
	j[JSON_CELL_OVERRIDE] = vecCells;
	j[JSON_SPAWNER_COORD] = spawnerCoord;
	j[JSON_BASE_COORD]	  = baseCoord;
	return j;
}

void Component_Hexgrid::FromJson(const json &_j)
{
	if (_j.contains(JSON_WIDTH))
		uWidth = _j[JSON_WIDTH];
	if (_j.contains(JSON_HEIGHT))
		uHeight = _j[JSON_HEIGHT];
	if (_j.contains(JSON_CELL_OVERRIDE))
		_j[JSON_CELL_OVERRIDE].get_to(vecCells);
	if (_j.contains(JSON_SPAWNER_COORD))
		spawnerCoord = _j[JSON_SPAWNER_COORD];
	if (_j.contains(JSON_BASE_COORD))
		baseCoord = _j[JSON_BASE_COORD];
}

void Component_Hexgrid::AfterLoad()
{
	RegenerateGrid();
}

namespace HexgridSystem
{
	Hexgrid *pHexgrid = nullptr;

	void OnStart(entt::registry &_registry)
	{
		_registry.view<Component_Hexgrid>().each([&](entt::entity _enttID, Component_Hexgrid &_gridComp) {
			ASSERTM(pHexgrid == nullptr, "Hexgrid already found, only one is needed");
			pHexgrid = &_gridComp.m_grid;
			// TODO register some callback for when the entity/component is destroyed, so we dont keep an invalid pointer
			// or use a shared_pointer...
		});
	}

	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry)
	{
		PROFILING_FUNCTION();

		if (pHexgrid == nullptr)
			return;

		if (Plop::Input::IsMouseLeftPressed())
		{
			auto xLevel	 = Plop::Application::GetCurrentLevel().lock();
			auto xCamera = xLevel->GetCamera().lock();

			// TODO viewport panel
			auto &			editor		 = Plop::Application::Get()->GetEditor();
			const glm::vec2 vViewportPos = editor.GetViewportPosFromWindowPos(Plop::Input::GetCursorWindowPos());
			glm::vec3		vMousePos	 = xCamera->GetWorldPosFromViewportPos(vViewportPos, 0.f);

			Hexgrid::CellCoord coord = Hexgrid::Cell::GetCellCoordFrom2D(vMousePos.xy);
			Plop::Log::Info("Coords: {},{},{} -> {},{},{}", vMousePos.x, vMousePos.y, vMousePos.z, coord.x, coord.y, coord.z);
			Plop::Log::Info("	Dist: {}", glm::manhattanDistance(coord, Hexgrid::CellCoord(0, 0, 0)) / 2.f);

			Hexgrid::Cell cellClick;
			if (pHexgrid->GetCell(coord, &cellClick) && cellClick.eType == CellType::TOWER_SUPPORT)
			{
				bool bOverlap = false;
				auto view	  = xLevel->GetEntityRegistry().view<Plop::Component_Transform, Component_Tower>();
				for (auto enttID : view)
				{
					auto &transform = view.get<Plop::Component_Transform>(enttID);

					if (bOverlap)
						return;

					if (glm::length2(transform.GetWorldPosition() - vMousePos) < 1)
					{
						bOverlap = true;
						break;
					}
				}

				if (!bOverlap)
				{
					const Plop::GUID guidTower = 8392535099134412618llu;
					Plop::Entity	 tower	   = Plop::PrefabManager::InstantiatePrefab(guidTower, xLevel->GetEntityRegistry(), Plop::Entity());
					tower.GetComponent<Plop::Component_Transform>().SetWorldPosition(glm::vec3(Hexgrid::Cell::Get2DCoordFromCell(coord), 1.f));
				}
			}
		}
	}

	void OnStop(entt::registry &_registry) { pHexgrid = nullptr; }

	void OnUpdateEditor(const Plop::TimeStep &_ts, entt::registry &_registry)
	{
		if (Plop::Input::IsMouseLeftPressed())
		{
			if (Private::ePaintType != CellType::INVALID)
			{
				auto &editor  = Plop::Application::Get()->GetEditor();
				auto xCamera = editor.GetEditorCamera();

				glm::vec2 vScreenPos = Plop::Input::GetCursorWindowPos();
				vScreenPos			 = editor.GetViewportPosFromWindowPos(vScreenPos);
				glm::vec3 vMousePos	 = xCamera->GetWorldPosFromViewportPos(vScreenPos, 0.f);


				Hexgrid::Cell *pCellClick = nullptr;
				if (Private::pEditingComp->m_grid.GetModifiableCell(Hexgrid::Cell::GetCellCoordFrom2D(vMousePos.xy), &pCellClick))
				{
					auto it = std::find(Private::pEditingComp->vecCells.begin(), Private::pEditingComp->vecCells.end(), *pCellClick);
					if (it == Private::pEditingComp->vecCells.end())
					{
						pCellClick->eType = Private::ePaintType;
						pCellClick->ApplyCellType();
						Private::pEditingComp->vecCells.push_back(*pCellClick);
					}
					else
					{
						Hexgrid::Cell *pCell = nullptr;
						if (Private::pEditingComp->m_grid.GetModifiableCell(it->coord, &pCell))
						{
							it->eType	 = Private::ePaintType;
							pCell->eType = Private::ePaintType;
							pCell->ApplyCellType();
						}
					}

					if (!Plop::Input::IsKeyDown(Plop::KeyCode::KEY_Shift))
					{
						Private::ePaintType	  = CellType::INVALID;
						Private::pEditingComp = nullptr;
						Plop::Application::Get()->GetEditor().RestorePicking();
					}
				}
			}
			else if (Private::ePickingMode != Private::PickingMode::NONE)
			{
				auto &editor  = Plop::Application::Get()->GetEditor();
				auto xCamera = editor.GetEditorCamera();

				glm::vec2 vScreenPos = Plop::Input::GetCursorWindowPos();
				vScreenPos			 = editor.GetViewportPosFromWindowPos(vScreenPos);
				glm::vec3 vMousePos	 = xCamera->GetWorldPosFromViewportPos(vScreenPos, 0.f);


				Hexgrid::Cell	   cellClick;
				Hexgrid::CellCoord hexCoord = Hexgrid::Cell::GetCellCoordFrom2D(vMousePos.xy);
				if (Private::pEditingComp->m_grid.GetCell(hexCoord, &cellClick))
				{
					if (Private::ePickingMode == Private::PickingMode::CELL)
					{
						auto it = std::find(Private::pEditingComp->vecCells.begin(), Private::pEditingComp->vecCells.end(), cellClick);
						if (it == Private::pEditingComp->vecCells.end())
						{
							Private::pEditingComp->vecCells.push_back(cellClick);

							if (!Plop::Input::IsKeyDown(Plop::KeyCode::KEY_Shift))
							{
								Private::ePickingMode = Private::PickingMode::NONE;
								Private::pEditingComp = nullptr;
								Plop::Application::Get()->GetEditor().RestorePicking();
							}
						}
					}
					else if (Private::ePickingMode == Private::PickingMode::SPAWNER)
					{
						Private::pEditingComp->m_grid.SetSpawnerCoord(hexCoord);
						Private::pEditingComp->spawnerCoord = hexCoord;
						Private::ePickingMode				= Private::PickingMode::NONE;
						Private::pEditingComp				= nullptr;
						Plop::Application::Get()->GetEditor().RestorePicking();
					}
					else if (Private::ePickingMode == Private::PickingMode::BASE)
					{
						Private::pEditingComp->m_grid.SetBaseCoord(hexCoord);
						Private::pEditingComp->baseCoord = hexCoord;
						Private::ePickingMode			 = Private::PickingMode::NONE;
						Private::pEditingComp			 = nullptr;
						Plop::Application::Get()->GetEditor().RestorePicking();
					}
				}
			}
		}
	}
} // namespace HexgridSystem