#include <PlopInclude.h>

#include <imgui_entt_entity_editor.hpp>

#include "Debug/Log.h"

#include "SampleApp.h"
#include "SampleLayer.h"
#include "SampleLevel.h"
#include "SampleComponents.h"


Plop::Application* Plop::CreateApplication(const StringVec& _Arguments)
{
	return NEW SampleApp( _Arguments );
}

SampleApp::SampleApp( const StringVec& _Arguments )
	: Application( _Arguments )
{
}

void SampleApp::Init()
{
	Plop::Application::Init();

	RegisterAppLayer( NEW SampleLayer() );
	RegisterAppLayer( NEW SampleLayer2D() );
}

Plop::GameConfig* SampleApp::CreateGameConfig()
{
	return NEW SampleConfig();
}

Plop::LevelBasePtr SampleApp::CreateNewLevelPrivate()
{
	Plop::LevelBasePtr xLevel = std::make_shared<SampleLevel>();
	return xLevel;
}

void SampleApp::RegisterComponents()
{
	REGISTER_COMPONENT( Rotating );
}