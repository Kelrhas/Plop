#pragma once

#include <Renderer/Sprite.h>

class Level
{
public:
	Level();
	~Level();

	void				Init();

	void				LoadFromString( const char* _pLevel, uint32_t _uWidth, uint32_t _uHeight );

	void				Draw() const;

private:
	// TODO: handle chunks
	Plop::TexturePtr	m_xSpritesheet = nullptr;
	uint32_t			m_uWidth = 0;
	uint32_t			m_uHeight = 0;
	Plop::Sprite***		m_ppSprites = nullptr;
};

