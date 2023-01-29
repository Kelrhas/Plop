#include "TD_pch.h"

#include "Hexgrid.h"

#include <Assets/SpritesheetLoader.h>
#include <ECS/Components/Component_EnemySpawner.h>
#include <ECS/Components/Component_PlayerBase.h>
#include <ECS/Components/Component_SpriteRenderer.h>
#include <ECS/Components/Component_Transform.h>
#include <Math/Math.h>
#include <Utils/JsonTypes.h>
#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122

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
	switch (eType)
	{
		case CellType::PATH: return 1.f;
		case CellType::WALL: return -1.f;
		case CellType::TOWER_SUPPORT: return -1.f;
	}

	return 1.f;
}

void Hexgrid::Cell::ApplyCellType() const
{
	auto &spriteComp = entity.GetComponent<Plop::Component_SpriteRenderer>();

	switch (eType)
	{
		case CellType::PATH: spriteComp.xSprite->SetTint(COLOR_WHITE); break;
		case CellType::WALL: spriteComp.xSprite->SetTint(COLOR_BLACK); break;
		case CellType::TOWER_SUPPORT: spriteComp.xSprite->SetTint(COLOR_RED); break;
		case CellType::INVALID:
		default: spriteComp.xSprite->SetTint(COLOR_GREY192); break;
	}
}

void Hexgrid::Init(U32 _uWidth, U32 _uHeight, Plop::Entity _container)
{
	Plop::LevelBasePtr xLevel = Plop::Application::GetCurrentLevel().lock();
	StringPath sSpritesheet = std::filesystem::canonical("assets/textures/hexagons.ssdef");
	auto hSpritesheet = Plop::AssetLoader::GetSpritesheet(sSpritesheet);

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
			pCell->eType = CellType::PATH;

			sprintf(sName.data(), "Tile_%d_%d", x, y);
			pCell->entity = xLevel->CreateEntity(sName);
			pCell->entity.AddFlag(Plop::EntityFlag::DYNAMIC_GENERATION);
			pCell->entity.SetParent(_container);
			auto &transform = pCell->entity.GetComponent<Plop::Component_Transform>();
			transform.SetLocalPosition(glm::vec3(Cell::Get2DCoordFromCell(coord), 0));

			auto &spriteComp = pCell->entity.AddComponent<Plop::Component_SpriteRenderer>();
			spriteComp.xSprite->SetSpritesheet(hSpritesheet, VEC2_0);

			pCell->ApplyCellType();
		}
	}
}

void Hexgrid::Reset()
{
	m_grid.clear();
}

void Hexgrid::SetBaseCoord(const CellCoord &_coord)
{
	auto xLevel = Plop::Application::Get()->GetCurrentLevel().lock();

	if (!m_BaseEntity)
	{
		auto &reg = xLevel->GetEntityRegistry();
		reg.view<Component_PlayerBase>().each([&](entt::entity _enttID, Component_PlayerBase &_base) {
			ASSERTM(!m_BaseEntity || m_BaseEntity == _enttID, "PlayerBase entity already found");
			m_BaseEntity = Plop::Entity(_enttID, reg);
		});
	}

	if (!m_BaseEntity)
	{
		m_BaseEntity = xLevel->CreateEntity("Base");
		auto &baseComp = m_BaseEntity.AddComponent<Component_PlayerBase>();
		baseComp.fLife = 10.f;

		StringPath				sSpritesheet   = std::filesystem::canonical("assets/textures/tiles.ssdef");
		Plop::SpritesheetHandle hSpritesheet   = Plop::AssetLoader::GetSpritesheet(sSpritesheet);
		auto &					baseSpriteComp = m_BaseEntity.AddComponent<Plop::Component_SpriteRenderer>();
		baseSpriteComp.xSprite->SetSpritesheet(hSpritesheet, "player_base");
	}

	glm::vec3 vPos = glm::vec3(Hexgrid::Cell::Get2DCoordFromCell(_coord), 1.f);
	m_BaseEntity.GetComponent<Plop::Component_Transform>().SetWorldPosition(vPos);

	GeneratePath();
}

void Hexgrid::SetSpawnerCoord(const CellCoord &_coord)
{
	auto xLevel = Plop::Application::Get()->GetCurrentLevel().lock();

	if (!m_SpawnerEntity)
	{
		auto &reg = xLevel->GetEntityRegistry();
		reg.view<Component_EnemySpawner>().each([&](entt::entity _enttID, const Component_EnemySpawner &) {
			ASSERTM(!m_SpawnerEntity || m_SpawnerEntity == _enttID, "Spawner entity already found");
			m_SpawnerEntity = Plop::Entity(_enttID, reg);
		});
	}

	if (!m_SpawnerEntity)
	{
		m_SpawnerEntity = xLevel->CreateEntity("Spawner");
		m_SpawnerEntity.AddComponent<Component_EnemySpawner>();

		StringPath				sSpritesheet	  = std::filesystem::canonical("assets/textures/tiles.ssdef");
		Plop::SpritesheetHandle hSpritesheet	  = Plop::AssetLoader::GetSpritesheet(sSpritesheet);
		auto &					spawnerSpriteComp = m_SpawnerEntity.AddComponent<Plop::Component_SpriteRenderer>();
		spawnerSpriteComp.xSprite->SetSpritesheet(hSpritesheet, "0");
	}

	glm::vec3 vPos = glm::vec3(Hexgrid::Cell::Get2DCoordFromCell(_coord), 1.f);
	m_SpawnerEntity.GetComponent<Plop::Component_Transform>().SetWorldPosition(vPos);

	GeneratePath();
}

void Hexgrid::GeneratePath()
{
	if (m_BaseEntity && m_SpawnerEntity)
	{
		glm::vec3		   vBasePos		= m_BaseEntity.GetComponent<Plop::Component_Transform>().GetWorldPosition();
		Hexgrid::CellCoord hexCoordBase = Hexgrid::Cell::GetCellCoordFrom2D(vBasePos);
		Hexgrid::Cell	   baseCell;
		GetCell(hexCoordBase, &baseCell);

		glm::vec3		   vSpawnerPos	   = m_SpawnerEntity.GetComponent<Plop::Component_Transform>().GetWorldPosition();
		Hexgrid::CellCoord hexCoordSpawner = Hexgrid::Cell::GetCellCoordFrom2D(vSpawnerPos);
		Hexgrid::Cell	   spawnerCell;
		GetCell(hexCoordSpawner, &spawnerCell);

		auto pf = GetPathfind(spawnerCell, baseCell);
		if (pf.bValid)
		{
			const float fPathDepth	= 0.f;
			auto &		spawnerComp = m_SpawnerEntity.GetComponent<Component_EnemySpawner>();
			spawnerComp.xPathCurve->vecControlPoints.clear();
			spawnerComp.xPathCurve->vecControlPoints.push_back(glm::vec3(Hexgrid::Cell::Get2DCoordFromCell(spawnerCell.coord), fPathDepth) - vSpawnerPos);


			glm::vec3 vLastCoord = glm::vec3(Hexgrid::Cell::Get2DCoordFromCell(pf.vecPath.front().coord), fPathDepth);
			for (auto &path : pf.vecPath)
			{
				const glm::vec3 vCoord = glm::vec3(Hexgrid::Cell::Get2DCoordFromCell(path.coord), fPathDepth);
				spawnerComp.xPathCurve->vecControlPoints.push_back((vCoord + vLastCoord) / 2.f - vSpawnerPos);
				vLastCoord = vCoord;
			}

			glm::vec3 vTarget = glm::vec3(Hexgrid::Cell::Get2DCoordFromCell(baseCell.coord), fPathDepth);
			spawnerComp.xPathCurve->vecControlPoints.push_back(vTarget - vSpawnerPos);

			m_BaseEntity.GetComponent<Plop::Component_Transform>().SetWorldPosition(vTarget - VEC3_FORWARD);
		}
	}
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

constexpr const char *JSON_CELL_COORD = "Coord";
constexpr const char *JSON_CELL_TYPE = "Type";

namespace nlohmann
{
	void adl_serializer<Hexgrid::Cell>::to_json(json &j, const Hexgrid::Cell &_cell)
	{
		j = json();
		j[JSON_CELL_COORD] = _cell.coord;
		j[JSON_CELL_TYPE] = _cell.eType;
	}
	void adl_serializer<Hexgrid::Cell>::from_json(const json &j, Hexgrid::Cell &_cell)
	{
		if (j.contains(JSON_CELL_COORD))
			_cell.coord = j[JSON_CELL_COORD];
		if (j.contains(JSON_CELL_TYPE))
			j[JSON_CELL_TYPE].get_to<CellType>(_cell.eType);
	}
} // namespace nlohmann