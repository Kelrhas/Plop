#include <PlopInclude.h>

#include "SampleApp.h"
#include <Debug/Log.h>

#include <SampleLayer.h>


Plop::Application* Plop::CreateApplication(const StringVec& _Arguments)
{
	return NEW SampleApp( _Arguments );
}

SampleApp::SampleApp( const StringVec& _Arguments )
	: Application( _Arguments )
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