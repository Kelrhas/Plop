#include "TD_pch.h"
#include "TDApp.h"

#include <Debug/Log.h>

#include "WorldLayer.h"
#include "Components/Tower.h"
#include "Components/Enemy.h"


Plop::Application* Plop::CreateApplication( const StringVec& _Arguments )
{
	return NEW TDApp( _Arguments );
}

TDApp::TDApp( const StringVec& _Arguments ) : Plop::Application( _Arguments )
{
}

void TDApp::Init()
{
	Plop::Application::Init();

	RegisterAppLayer( new WorldLayer() );
}

Plop::GameConfig* TDApp::CreateGameConfig()
{
	return new TDConfig();
}

Plop::LevelBasePtr TDApp::CreateNewLevelPrivate()
{
	return std::make_shared<TDLevel>();
}

void TDApp::RegisterComponents()
{
	REGISTER_COMPONENT( Tower );
	REGISTER_COMPONENT( Enemy );
}
