#include "TD_pch.h"
#include "Hexgrid.h"

#include <Assets/SpritesheetLoader.h>
#include <ECS/Components/Component_Transform.h>
#include <ECS/Components/Component_SpriteRenderer.h>
#include <Math/Math.h>

constexpr glm::mat2 mHex =		{Sqrt(3.0),			0.0,		Sqrt(3.0) / 2.0,		3.0 / 2.0};
constexpr glm::mat2 mHexInv =	{Sqrt(3.0) / 3.0,	0.0,		-1.0 / 3.0,				2.0 / 3.0};
constexpr glm::vec2 vRadius(0.5f, 0.5f);
//constexpr glm::vec2 vRadius(0.495f, 0.4953f); // depends on pixel UV ratio
constexpr glm::vec2 vOriginOffset = VEC2_0;

/*	NOTE:
	to render in a shader, use a SDF
	float hex(in vec2 p)
	{
		p = abs(p);

		const vec2 s = vec2(1, 1.7320508);
		return max(dot(p, s*.5), p.x); // Hexagon.
	}

	vec3 col = mix(vec3(1.), vec3(0), smoothstep(0., .01, eDist - .5));
	fragColor = vec4(col, 1);

	from https://www.shadertoy.com/view/wtdSzX
*/

glm::vec2 Hexgrid::Cell::Get2DCoordFromCell(const CellCoord &_coord)
{
	return mHex * _coord * vRadius + vOriginOffset;
}

Hexgrid::CellCoord Hexgrid::Cell::GetCellCoordFrom2D(const glm::vec2 &_coord)
{
	const glm::vec2 vFrac2D = mHexInv * ((_coord - vOriginOffset) / vRadius);

	const glm::vec3 vFrac(vFrac2D, -vFrac2D.x - vFrac2D.y);
	glm::vec3 vRounded = glm::round(vFrac);
	const glm::vec3 vDiff = glm::abs(vFrac - vRounded);

	if (vDiff.x > vDiff.y && vDiff.x > vDiff.z)
		vRounded.x = -vRounded.y - vRounded.z;
	else if (vDiff.y > vDiff.z)
		vRounded.y = -vRounded.x - vRounded.z;
	else
		vRounded.z = -vRounded.x - vRounded.y;

	return vRounded;
}

constexpr float Hexgrid::Cell::GetPathFindCost() const
{
	return 1.f;
}


void Hexgrid::Init(U32 _uWidth, U32 _uHeight)
{
	Plop::LevelBasePtr xLevel = Plop::Application::GetCurrentLevel().lock();
	StringPath sSpritesheet = std::filesystem::canonical(Plop::Application::Get()->GetRootDirectory() / "assets/textures/hexagons.ssdef");
	auto hSpritesheet = Plop::AssetLoader::GetSpritesheet(sSpritesheet);

	Plop::Entity grid = xLevel->CreateEntity("grid");
	grid.AddFlag(Plop::EntityFlag::DYNAMIC_GENERATION);

	String sName(16, 0);
	m_grid.resize(_uHeight);
	for (U32 y = 0; y < _uHeight; ++y)
	{
		m_grid[y].resize(_uWidth);
		for (U32 x = 0; x < _uWidth; ++x)
		{
			CellCoord coord;
			coord.x = x - size_t(y / 2);
			coord.y = y;
			coord.z = -coord.x - coord.y;

			Cell *pCell = nullptr;
			VERIFY(GetModifiableCell(coord, &pCell));
			pCell->coord = coord;

			sprintf(sName.data(), "Tile_%d_%d", x, y);
			Plop::Entity entity = xLevel->CreateEntity(sName);
			entity.AddFlag(Plop::EntityFlag::DYNAMIC_GENERATION);
			entity.SetParent(grid);
			auto &transform = entity.GetComponent<Plop::Component_Transform>();
			transform.SetLocalPosition(glm::vec3(Cell::Get2DCoordFromCell(coord), 0));

			auto &spriteComp = entity.AddComponent<Plop::Component_SpriteRenderer>();
			spriteComp.xSprite->SetSpritesheet(hSpritesheet, VEC2_0);
		}
	}
}

void Hexgrid::Reset()
{
	m_grid.clear();
}

bool Hexgrid::GetCell(const CellCoord &_coord, Cell *_pCellOut)
{
	if (!_pCellOut)
		return false;

	if (_coord.y >= 0 && _coord.y < m_grid.size())
	{
		size_t x = _coord.x + size_t(_coord.y / 2);
		if (x >= 0 && x < m_grid[_coord.y].size())
		{
			*_pCellOut = m_grid[_coord.y][x];
			return true;
		}
	}

	return false;
}

GridPathFind<Hexgrid::Cell> Hexgrid::GetPathfind(const Hexgrid::Cell &_vStart, const Hexgrid::Cell &_vEnd)
{
	GridPathFind<Cell> pf = A_Star(_vStart, _vEnd,
										[](const Cell &_cell1, const Cell &_cell2) -> float{
		CellCoord v1 = _cell1.coord;
		CellCoord v2 = _cell2.coord;
		return (float)glm::manhattanDistance(v1, v2) / 2.f;
	},
	[&](const Cell &_cell, std::vector<Cell> &_outVec) {

		Cell cell;
		if (GetCell(_cell.coord + CellCoord(1, 0, -1), &cell))
			_outVec.push_back(cell);
		if (GetCell(_cell.coord + CellCoord(1, -1, 0), &cell))
			_outVec.push_back(cell);
		if (GetCell(_cell.coord + CellCoord(0, -1, 1), &cell))
			_outVec.push_back(cell);
		if (GetCell(_cell.coord + CellCoord(-1, 0, 1), &cell))
			_outVec.push_back(cell);
		if (GetCell(_cell.coord + CellCoord(-1, 1, 0), &cell))
			_outVec.push_back(cell);
		if (GetCell(_cell.coord + CellCoord(0, 1, -1), &cell))
			_outVec.push_back(cell);
	});

	/** /
	if (pf.bValid)
	{
		Plop::Log::Info( "Valid from {}, {} to {}, {} with a cost of {}", _vStart.coord.x, _vStart.coord.y, _vEnd.coord.x, _vEnd.coord.y, pf.fTotalCost );
		for (auto& tile : pf.vecPath)
		{
			Plop::Log::Info( " - {}, {}", tile.coord.x, tile.coord.y );
		}
	}
	else
	{
		Plop::Log::Info( "NOT Valid from {}, {} to {}, {}", _vStart.coord.x, _vStart.coord.y, _vEnd.coord.x, _vEnd.coord.y );
	}
	/**/

	return pf;
}

bool Hexgrid::GetModifiableCell(const CellCoord &_coord, Cell **_pCellOut)
{
	if (!_pCellOut)
		return false;

	if (_coord.y >= 0 && _coord.y < m_grid.size())
	{
		size_t x = _coord.x + size_t(_coord.y / 2);
		if (x >= 0 && x < m_grid[_coord.y].size())
		{
			*_pCellOut = &m_grid[_coord.y][x];
			return true;
		}
	}

	return false;
}
