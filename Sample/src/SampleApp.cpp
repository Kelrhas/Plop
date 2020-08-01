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
}

Plop::GameConfig* SampleApp::CreateGameConfig()
{
	return new SampleConfig();
}