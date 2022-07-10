#pragma once

#include <glm/gtx/hash.hpp>

#include <ECS/Entity.h>
#include <ECS/Pathfind.h>
#include <Renderer/Spritesheet.h>



class LevelGrid
{
public:
	struct TileDefinition
	{
		enum
		{
			TYPE_NONE = -1,
			TYPE_GROUND,
			TYPE_WALL,
			TYPE_HARD_GROUND,

			TYPE_COUNT
		} eType = TYPE_NONE;
		

		Plop::Entity entity{};
		glm::uvec2 vCoord;

		constexpr float GetPathFindCost() const;




		bool operator==( const TileDefinition& _o ) const { return vCoord == _o.vCoord; }
		bool operator!=( const TileDefinition& _o ) const { return !(*this == _o); }
	};

	struct LevelConstraints
	{
		U32 m_uLevelWidth;
		U32 m_uLevelHeight;
		std::vector<glm::uvec2> m_vecStarts;
		//std::vector<glm::uvec2> m_vecEnds;
		glm::uvec2 m_vEnd;
		std::vector<TileDefinition> m_vecFixedTiles;

		bool IsValid() const;
	};


	void Init( const LevelConstraints& _constraints );
	void Reset();

	TileDefinition& GetTile( U32 x, U32 y ) { return m_pLevelGrid[x + uLevelWidth * y]; }
	TileDefinition& GetTile( const glm::uvec2& _vCoord ) { return m_pLevelGrid[_vCoord.x + uLevelWidth * _vCoord.y]; }

	GridPathFind<TileDefinition> GetPathfind( const TileDefinition& _vStart, const TileDefinition& _vEnd );


	U32 uLevelWidth = 10;
	U32 uLevelHeight = 10;

private:

	StringPath m_sSpritesheet;
	Plop::SpritesheetHandle m_hSpritesheet;
	TileDefinition* m_pLevelGrid = nullptr;

	const TileDefinition& GetTileDefinition( const String& _sTileName );
};



namespace std
{
	template<>
	struct hash<LevelGrid::TileDefinition>
	{
		using argument_type = LevelGrid::TileDefinition;
		using result_type = std::size_t;
		result_type operator()( argument_type const& in ) const
		{
			size_t hash = std::hash<glm::uvec2>()( in.vCoord );
			return hash;
		}
	};
}