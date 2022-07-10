#pragma once

#include <glm/gtx/hash.hpp>

#include <ECS/Pathfind.h>

/// https://www.redblobgames.com/grids/hexagons/
class Hexgrid
{
public:

	using CellCoord = glm::ivec3;
	struct Cell
	{
		CellCoord coord;

		static /*constexpr*/ glm::vec2 Get2DCoordFromCell(const CellCoord &_coord);
		static /*constexpr*/ CellCoord GetCellCoordFrom2D(const glm::vec2 &_coord);

		constexpr float GetPathFindCost() const;


		bool operator==(const Cell &_o) const { return coord == _o.coord; }
		bool operator!=(const Cell &_o) const { return !(*this == _o); }
	};


public:
	Hexgrid() = default;

	void	Init(U32 _uWidth, U32 _uHeight);
	void	Reset();


	size_t		GetHeight() const { return m_grid.size(); }

	bool GetCell(const CellCoord &_coord, Cell *_pCellOut);

	GridPathFind<Cell> GetPathfind(const Cell &_vStart, const Cell &_vEnd);


private:
	using RowVec = std::vector<Cell>;

	bool GetModifiableCell(const CellCoord &_coord, Cell **_pCellOut);

	std::vector<RowVec> m_grid;

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