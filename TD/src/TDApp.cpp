#include "TD_pch.h"
#include "TDApp.h"

#include <Debug/Log.h>
#include <ECS/ComponentManager.h>

#include "WorldLayer.h"
#include "ECS/Components/TD_ComponentIncludes.h"


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
//#define MACRO_COMPONENT(comp) RegisterComponent<Component_##comp, true>( #comp )
#define MACRO_COMPONENT(comp) Plop::ComponentManager::RegisterComponent<Component_##comp>( #comp );
#include "ECS/Components/TD_ComponentList.h"
#undef MACRO_COMPONENT
}
