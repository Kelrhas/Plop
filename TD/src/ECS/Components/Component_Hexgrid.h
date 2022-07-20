#pragma once

#include "ECS/Hexgrid.h"

struct Component_Hexgrid
{
	U32						   uWidth  = 16;
	U32						   uHeight = 10;
	std::vector<Hexgrid::Cell> vecCells;
	Hexgrid::CellCoord		   spawnerCoord;
	Hexgrid::CellCoord		   baseCoord;

	Hexgrid m_grid; // DO NOT SERIALISE

	void RegenerateGrid();

	void EditorUI();
	json ToJson() const;
	void FromJson(const json &_j);
	void AfterLoad();
};


namespace HexgridSystem
{
	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry);
	void OnUpdateEditor(const Plop::TimeStep &_ts, entt::registry &_registry);
}