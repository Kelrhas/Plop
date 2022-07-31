#pragma once

#include <Renderer/Sprite.h>
#include <ECS/LevelBase.h>

#include "ECS/Hexgrid.h"
#include "ECS/LevelGrid.h"

struct PlayerLevelStats
{
	int iMoney	= 0;
	int iNbKill = 0;
};


class TDLevel : public Plop::LevelBase
{
public:
	TDLevel();
	virtual ~TDLevel();

	virtual void				Init() override;
	virtual void				Shutdown() override;
	virtual bool				OnEvent(Plop::Event &_event) override;
	virtual void				OnStart() override;
	virtual void				Update(Plop::TimeStep& _ts) override;
	virtual void				OnStop() override;
	virtual void				UpdateInEditor(Plop::TimeStep _ts) override;

	//		void				LoadFromString( const char* _pLevel, uint32_t _uWidth, uint32_t _uHeight );


private:
	PlayerLevelStats m_playerStats;
};

