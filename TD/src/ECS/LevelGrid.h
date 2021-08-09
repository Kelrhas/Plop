#pragma once

#include <ECS/Entity.h>
#include <Renderer/Spritesheet.h>

class LevelGrid
{
public:
	struct TileDefinition
	{
		enum
		{
			TYPE_GROUND,
			TYPE_WALL,

			TYPE_COUNT
		} eType = TYPE_WALL;
		

		Plop::Entity entity{};

		constexpr float GetPathFindCost();
	};


	void Init();



private:

	StringPath m_sSpritesheet;
	Plop::SpritesheetHandle m_hSpritesheet;
	U32 uLevelWidth = 10;
	U32 uLevelHeight = 10;
	TileDefinition* levelGrid = nullptr;

	const TileDefinition& GetTileDefinition( const String& _sTileName );
};