#include <PlopInclude.h>

#include "SampleApp.h"
#include <Log.h>

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