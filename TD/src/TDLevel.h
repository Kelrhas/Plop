#pragma once

#include <Renderer/Sprite.h>
#include <ECS/LevelBase.h>
#include <Renderer/ParticleSystem.h>

class TDLevel : public Plop::LevelBase
{
public:
	TDLevel();
	virtual ~TDLevel();

	virtual void				Init() override;
	virtual void				Update(Plop::TimeStep _ts) override;

	//		void				LoadFromString( const char* _pLevel, uint32_t _uWidth, uint32_t _uHeight );


private:
	Plop::ParticleSystem		m_particles;
	// TODO: handle chunks
	//Plop::TexturePtr	m_xSpritesheet = nullptr;
	//uint32_t			m_uWidth = 0;
	//uint32_t			m_uHeight = 0;
	//Plop::Sprite***		m_ppSprites = nullptr;
};

