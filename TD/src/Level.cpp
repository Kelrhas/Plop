#include "TD_pch.h"
#include "Level.h"


#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>

const char* testLevel = 
"GGGGGGGG"
"GGDDDDGG"
"GGDDWDDG"
"GGDDDDGG"
"GGGGGGGG";

Level::Level()
{
}

Level::~Level()
{

}

void Level::Init()
{
	if (m_xSpritesheet == nullptr)
	{
		m_xSpritesheet = Plop::Texture::Create2D( "TD/assets/textures/tilesheet.png" );
	}

	LoadFromString( testLevel, 8, 5 );
}

void Level::LoadFromString( const char* _pLevel, uint32_t _uWidth, uint32_t _uHeight )
{
	m_uWidth = _uWidth;
	m_uHeight = _uHeight;

	m_ppSprites = new Plop::Sprite**[m_uWidth];
	for (uint32_t x = 0; x < m_uWidth; ++x)
	{
		m_ppSprites[x] = new Plop::Sprite*[m_uHeight];
		for (uint32_t y = 0; y < m_uHeight; ++y)
		{
			char c = _pLevel[y * m_uWidth + x];

			if (c == 'G')
			{
				m_ppSprites[x][y] = new Plop::Sprite();
				m_ppSprites[x][y]->SetSpriteIndex( { 1, 11 }, { 23, 13 } );
			}
			else if (c == 'D')
			{
				m_ppSprites[x][y] = new Plop::Sprite();
				m_ppSprites[x][y]->SetSpriteIndex( { 1, 8 }, { 23, 13 } );
			}
			else if (c == 'W')
			{
				m_ppSprites[x][y] = new Plop::Sprite();
				m_ppSprites[x][y]->SetSpriteIndex( { 11, 11 }, { 23, 13 } );
			}
			else
			{
				m_ppSprites[x][y] = nullptr;
				continue;
			}

			m_ppSprites[x][y]->SetTexture( m_xSpritesheet );
		}
	}
}

void Level::Draw() const
{
	PROFILING_FUNCTION();

	for (uint32_t x = 0; x < m_uWidth; ++x)
	{
		for (uint32_t y = 0; y < m_uHeight; ++y)
		{
			Plop::Sprite* pSprite = m_ppSprites[x][y];
			if (pSprite)
			{
				Plop::Renderer2D::DrawSprite( { x, m_uHeight - y - 1 }, { 1, 1 }, *pSprite );
			}
		}
	}
}
