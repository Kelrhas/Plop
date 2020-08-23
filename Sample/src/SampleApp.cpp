#include <PlopInclude.h>

#include "SampleApp.h"
#include <Debug/Log.h>

#include <SampleLayer.h>


Plop::Application* Plop::CreateApplication()
{
	return new SampleApp();
}

SampleApp::SampleApp()
{
}

Plop::GameConfig* SampleApp::CreateGameConfig()
{
	return new SampleConfig();
}

void SampleApp::Init()
{
	Plop::Application::Init();

	RegisterAppLayer( new SampleLayer() );
	RegisterAppLayer( new SampleLayer2D() );
}