#pragma once

#include <glm/gtx/hash.hpp>

#include <ECS/Pathfind.h>

/// https://www.redblobgames.com/grids/hexagons/

enum class CellType
{
	INVALID = -1,

	PATH,
	WALL,
	TOWER_SUPPORT,

	COUNT
};
NLOHMANN_JSON_SERIALIZE_ENUM(CellType, {
	{CellType::INVALID, nullptr},
	{CellType::PATH, "PATH"},
	{CellType::WALL, "WALL"},
	{CellType::TOWER_SUPPORT, "TOWER_SUPPORT"},
})

class Hexgrid
{
public:

	using CellCoord = glm::ivec3;
	struct Cell
	{
		CellCoord	 coord;
		CellType	 eType = CellType::INVALID;
		Plop::Entity entity;

		static /*constexpr*/ glm::vec2 Get2DCoordFromCell(const CellCoord &_coord);
		static /*constexpr*/ CellCoord GetCellCoordFrom2D(const glm::vec2 &_coord);

		constexpr float GetPathFindCost() const;
		void			ApplyCellType() const;


		bool operator==(const Cell &_o) const { return coord == _o.coord && eType == _o.eType; }
		bool operator!=(const Cell &_o) const { return !(*this == _o); }
	};


public:
	Hexgrid() = default;

	void	Init(U32 _uWidth, U32 _uHeight, Plop::Entity _container);
	void	Reset();

	void	SetBaseCoord(const CellCoord &_coord);
	void	SetSpawnerCoord(const CellCoord &_coord);

	void	GeneratePath();

	size_t			   GetHeight() const { return m_grid.size(); }
	bool			   GetCell(const CellCoord &_coord, Cell *_pCellOut);
	bool			   GetModifiableCell(const CellCoord &_coord, Cell **_pCellOut);
	GridPathFind<Cell> GetPathfind(const Cell &_vStart, const Cell &_vEnd);


private:
	using RowVec = std::vector<Cell>;


	std::vector<RowVec> m_grid;
	Plop::Entity		m_SpawnerEntity;
	Plop::Entity		m_BaseEntity;

};



namespace std
{
	template<>
	struct hash<Hexgrid::Cell>
	{
		using argument_type = Hexgrid::Cell;
		using result_type = std::size_t;
		result_type operator()(argument_type const &in) const
		{
			size_t hash = std::hash<decltype(argument_type::coord)>()(in.coord);
			return hash;
		}
	};
}

namespace nlohmann
{
	template<>
	struct adl_serializer<Hexgrid::Cell>
	{
		static void to_json(json &j, const Hexgrid::Cell &_cell);
		static void from_json(const json &j, Hexgrid::Cell &_cell);
	};
}