#include <PlopInclude.h>

#include "SampleApp.h"
#include <Debug/Log.h>

#include <SampleLayer.h>


Plop::Application* Plop::CreateApplication()
{
	return NEW SampleApp();
}

SampleApp::SampleApp()
{
}

Plop::GameConfig* SampleApp::CreateGameConfig()
{
	return NEW SampleConfig();
}

void SampleApp::Init()
{
	Plop::Application::Init();

	RegisterAppLayer( NEW SampleLayer() );
	RegisterAppLayer( NEW SampleLayer2D() );
}