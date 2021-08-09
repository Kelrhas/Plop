#include "TD_pch.h"
#include "LevelGrid.h"

#include <Application.h>
#include <ECS/Components/Component_SpriteRenderer.h>
#include <ECS/Components/Component_Transform.h>
#include <Assets/SpritesheetLoader.h>

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



constexpr float LevelGrid::TileDefinition::GetPathFindCost()
{
	switch (eType)
	{
		case TYPE_GROUND:	return 1.f;


		default:
			Plop::Debug::TODO();
			[[fallthrough]];
		case TYPE_WALL:	
			return -1.f;
	}
}

void LevelGrid::Init()
{

	uLevelWidth = 30;
	uLevelHeight = 30;


	m_sSpritesheet = std::filesystem::canonical( Plop::Application::Get()->GetRootDirectory() / "assets/textures/tiles.ssdef" );
	m_hSpritesheet = Plop::AssetLoader::GetSpritesheet( m_sSpritesheet );

	levelGrid = new TileDefinition[uLevelWidth * uLevelHeight];

	Plop::LevelBasePtr xLevel = Plop::LevelBase::GetCurrentLevel().lock();


	Plop::Entity grid = xLevel->CreateEntity( "grid" );

	glm::uvec2 vBasePosition( uLevelWidth - 2, uLevelHeight - 2 );
	glm::uvec2 vSpawnPosition = VEC2_1;

	float fHalfway = floor( (vBasePosition.x + vSpawnPosition.x) / 2.f );
	// generate the base level definition
	for (U32 y = 0; y < uLevelHeight; ++y)
	{
		for (U32 x = 0; x < uLevelWidth; ++x)
		{
			TileDefinition& tile = levelGrid[x + uLevelWidth * y];

			if (x < fHalfway)
			{
				if (y == vSpawnPosition.y)
					tile.eType = TileDefinition::TYPE_GROUND;
			}
			else if (x == fHalfway)
			{
				if (y >= vSpawnPosition.y && y <= vBasePosition.y)
					tile.eType = TileDefinition::TYPE_GROUND;
			}
			else
			{
				if (y == vBasePosition.y)
					tile.eType = TileDefinition::TYPE_GROUND;
			}
		}
	}

	/**/
	// generate entities
	for (U32 y = 0; y < uLevelHeight; ++y)
	{
		for (U32 x = 0; x < uLevelWidth; ++x)
		{
			TileDefinition& tile = levelGrid[x + uLevelWidth * y];
			String sName;
			sName.resize( 16 );
			sprintf(sName.data(), "Tile_%d_%d", x, y);
			tile.entity = xLevel->CreateEntity( sName );
			tile.entity.SetParent( grid );
			auto& transform = tile.entity.GetComponent<Plop::Component_Transform>();
			transform.SetLocalPosition( glm::vec3( x, y, 0 ) );
			/*
			if (bTransition)
			{
				float fAngle = 0;
				if (x > 1)
					fAngle += glm::half_pi<float>();

				if (y < uLevelHeight -2 && x > 1)
					fAngle += glm::half_pi<float>();

				if (y == 1 && !bCorner)
					fAngle += glm::half_pi<float>();

				if(x == 1 && y == 1)
					fAngle -= glm::half_pi<float>();

				if(fAngle != 0.f)
					transform.SetLocalRotation( glm::angleAxis( fAngle, VEC3_FORWARD ) );
			}
			*/

			auto& spriteComp = tile.entity.AddComponent<Plop::Component_SpriteRenderer>();
			spriteComp.xSprite->SetSpritesheet( m_hSpritesheet, tile.eType == TileDefinition::TYPE_GROUND ? "concrete" : "grass" );
		}
	}
	/**/
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