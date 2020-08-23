#include "TD_pch.h"
#include "TDApp.h"

#include <Debug/Log.h>

#include <WorldLayer.h>


Plop::Application* Plop::CreateApplication()
{
	return new TDApp();
}

TDApp::TDApp()
{
}

Plop::GameConfig* TDApp::CreateGameConfig()
{
	return new TDConfig();
}

void TDApp::Init()
{
	Plop::Application::Init();

	RegisterAppLayer( new WorldLayer() );
}
