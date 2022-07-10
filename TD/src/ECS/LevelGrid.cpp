#include "TD_pch.h"
#include "LevelGrid.h"

#include <Application.h>
#include <ECS/Components/Component_SpriteRenderer.h>
#include <ECS/Components/Component_Transform.h>
#include <Assets/SpritesheetLoader.h>

#include <Debug/Log.h>

namespace // private
{
	/*const std::map<String, LevelTileDefinition> s_mapTilesDef =
	{
		{
			String( "ground" ), { 1.f, String( "\assets\textures\tiles.ssdef" ), String("gravel")}
		}
	};*/
	//LevelTileDefinition s_errorDefinition{ -1.f, "error", {-1, -1} };
}



constexpr float LevelGrid::TileDefinition::GetPathFindCost() const
{
	switch (eType)
	{
		case TYPE_GROUND:		return 1.f;
		case TYPE_HARD_GROUND:	return 5.f;


		default:
			Plop::Debug::TODO();
			[[fallthrough]];
		case TYPE_WALL:	
			return -1.f;
	}
}


bool LevelGrid::LevelConstraints::IsValid() const
{
	if (m_vecStarts.empty() /*|| m_vecEnds.size()*/)
		return false;

	for (const auto& vStart : m_vecStarts)
		if (vStart.x >= m_uLevelWidth || vStart.y >= m_uLevelHeight)
			return false;
	//for (const auto& vEnd : m_vecEnds)
	//	if (vEnd.x >= m_uLevelWidth || vEnd.y >= m_uLevelHeight)
	//		return false;
	if (m_vEnd.x >= m_uLevelWidth || m_vEnd.y >= m_uLevelHeight)
		return false;

	for (const auto& tiles : m_vecFixedTiles)
		if (tiles.vCoord.x >= m_uLevelWidth || tiles.vCoord.y >= m_uLevelHeight)
			return false;

	return true;
}



void LevelGrid::Init( const LevelConstraints& _constraints )
{
	ASSERTM( m_pLevelGrid == nullptr, "Grid not null, call Reset beforehand" );
	ASSERTM( _constraints.IsValid(), "The constraints are not valid" );

	uLevelWidth = _constraints.m_uLevelWidth;
	uLevelHeight = _constraints.m_uLevelHeight;

	m_pLevelGrid = new TileDefinition[uLevelWidth * uLevelHeight];

	for (const TileDefinition& fixedTile : _constraints.m_vecFixedTiles)
	{
		auto& tile = GetTile( fixedTile.vCoord );
		tile = fixedTile;
	}


	const glm::uvec2& vSpawnPosition = _constraints.m_vecStarts.front();

	float fHalfway = floor( (_constraints.m_vEnd.x + vSpawnPosition.x) / 2.f );

	//// generate the base level definition
	for (U32 y = 0; y < uLevelHeight; ++y)
	{
		for (U32 x = 0; x < uLevelWidth; ++x)
		{
			TileDefinition& tile = GetTile( x, y );
			if (tile.eType == TileDefinition::TYPE_NONE)
			{
				tile.vCoord = glm::uvec2( x, y );
				tile.eType = TileDefinition::TYPE_WALL;

				if (x < fHalfway)
				{
					if (y == vSpawnPosition.y)
						tile.eType = TileDefinition::TYPE_GROUND;
				}
				else if (x == fHalfway)
				{
					if (y >= vSpawnPosition.y && y <= _constraints.m_vEnd.y)
						tile.eType = TileDefinition::TYPE_GROUND;
				}
				else
				{
					if (y == _constraints.m_vEnd.y)
						tile.eType = TileDefinition::TYPE_GROUND;
				}
			}
		}
	}



	//// generate entities according to the definitions generated

	m_sSpritesheet = std::filesystem::canonical("assets/textures/tiles.ssdef");
	m_hSpritesheet = Plop::AssetLoader::GetSpritesheet( m_sSpritesheet );


	Plop::LevelBasePtr xLevel = Plop::Application::GetCurrentLevel().lock();
	Plop::Entity grid = xLevel->CreateEntity( "grid" );
	grid.AddFlag( Plop::EntityFlag::DYNAMIC_GENERATION );

	String sName;
	sName.resize( 16 );
	for (U32 y = 0; y < uLevelHeight; ++y)
	{
		for (U32 x = 0; x < uLevelWidth; ++x)
		{
			TileDefinition& tile = GetTile(x, y);
			sName.clear();
			sprintf(sName.data(), "Tile_%d_%d", x, y);
			tile.entity = xLevel->CreateEntity( sName );
			tile.entity.AddFlag( Plop::EntityFlag::DYNAMIC_GENERATION );
			tile.entity.SetParent( grid );
			auto& transform = tile.entity.GetComponent<Plop::Component_Transform>();
			transform.SetLocalPosition( glm::vec3( x, y, 0 ) );


			auto& spriteComp = tile.entity.AddComponent<Plop::Component_SpriteRenderer>();
			spriteComp.xSprite->SetSpritesheet( m_hSpritesheet, tile.eType == TileDefinition::TYPE_GROUND ? "concrete" : "grass" );
		}
	}
}

void LevelGrid::Reset()
{
	delete[] m_pLevelGrid;
	m_pLevelGrid = nullptr;
}


GridPathFind<LevelGrid::TileDefinition> LevelGrid::GetPathfind( const LevelGrid::TileDefinition& _vStart, const LevelGrid::TileDefinition& _vEnd )
{
	using CellType = LevelGrid::TileDefinition;

	GridPathFind<CellType> pf = A_Star( _vStart, _vEnd,
		[]( const CellType& _cell1, const CellType& _cell2 ) {
			glm::vec2 v1 = _cell1.vCoord;
			glm::vec2 v2 = _cell2.vCoord;
			return glm::distance2( v1, v2 );
		},
		[&]( const CellType& _cell, std::vector<CellType>& _outVec ) 		{
			
			if (_cell.vCoord.x > 0)
				_outVec.push_back( GetTile( _cell.vCoord.x - 1, _cell.vCoord.y ) );
			if (_cell.vCoord.x < uLevelWidth - 1)
				_outVec.push_back( GetTile( _cell.vCoord.x + 1, _cell.vCoord.y ) );
			if (_cell.vCoord.y > 0)
				_outVec.push_back( GetTile( _cell.vCoord.x, _cell.vCoord.y - 1 ) );
			if (_cell.vCoord.y < uLevelHeight - 1)
				_outVec.push_back( GetTile( _cell.vCoord.x, _cell.vCoord.y + 1 ) );
			
		} );
	/*
	if (pf.bValid)
	{
		Plop::Log::Info( "Valid from {}, {} to {}, {} with a cost of {}", _vStart.vCoord.x, _vStart.vCoord.y, _vEnd.vCoord.x, _vEnd.vCoord.y, pf.fTotalCost );
		for (auto& tile : pf.vecPath)
		{
			Plop::Log::Info( " - {}, {}", tile.vCoord.x, tile.vCoord.y );
		}
	}
	else
	{
		Plop::Log::Info( "NOT Valid from {}, {} to {}, {}", _vStart.vCoord.x, _vStart.vCoord.y, _vEnd.vCoord.x, _vEnd.vCoord.y );
	}
	*/

	return pf;
}

const LevelGrid::TileDefinition& LevelGrid::GetTileDefinition( const String& _sTileName )
{
	static TileDefinition s_errorDefinition;/* { -1.f, "error" };
	static const std::map<String, TileDefinition> s_mapTilesDef =
	{
		{	String( "ground" ),				TileDefinition{1.f, String( "concrete" )}},
		{	String( "grass" ),				TileDefinition{-1.f, String( "grass" )}},
		{	String( "interior_corner" ),	TileDefinition{-1.f, String( "concrete-grass_interior_corner" )}},
		{	String( "exterior_corner" ),	TileDefinition{-1.f, String( "concrete-grass_exterior_corner" )}},
		{	String( "side" ),				TileDefinition{-1.f, String( "concrete-grass_side" )}},
	};

	const auto& it = s_mapTilesDef.find( _sTileName );
	ASSERT( it != s_mapTilesDef.end() );
	if (it != s_mapTilesDef.end())
	{
		return it->second;
	}
	*/
	return s_errorDefinition;
}