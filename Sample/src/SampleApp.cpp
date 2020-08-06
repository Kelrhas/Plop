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
	RegisterAppLayer(new SampleLayer());
	RegisterAppLayer(new SampleLayer2D());
}

Plop::GameConfig* SampleApp::CreateGameConfig()
{
	return new SampleConfig();
}