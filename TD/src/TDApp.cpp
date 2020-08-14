#include <PlopInclude.h>

#include "TDApp.h"
#include <Debug/Log.h>

#include <WorldLayer.h>


Plop::Application* Plop::CreateApplication()
{
	return new TDApp();
}

TDApp::TDApp()
{
	RegisterAppLayer(new WorldLayer());
}

Plop::GameConfig* TDApp::CreateGameConfig()
{
	return new TDConfig();
}