#include "Plop_pch.h"
#include <PlopInclude.h>

#include <fstream>

#include <json.hpp>

#include <Input/Input.h>
#include <Debug/Debug.h>
#include <Debug/Log.h>
#include <Events/EventDispatcher.h>
#include <Events/WindowEvent.h>

#include <Renderer/Renderer.h>

namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	/// Config

	const char* Config::CONFIG_FILE_NAME = "Config.json";

	void to_json( nlohmann::json& j, const WindowConfig& _config )
	{
		j = nlohmann::json
		{
			{ "width", _config.uWidth},
			{ "height", _config.uHeight },
			{ "pos x", _config.uPosX },
			{ "pos y", _config.uPosY },
			{ "fullscreen", _config.bFullscreen }
		};
	}

	void to_json( nlohmann::json& j, const Config& _config )
	{
		j = nlohmann::json
		{
			{"lastLevel", _config.sLastLevelActive},
			{"windows", _config.windows}
		};
	}

	void from_json( const nlohmann::json& j, WindowConfig& _config )
	{
		if (j.contains( "width" )) j.at( "width" ).get_to( _config.uWidth );
		if (j.contains( "height" )) j.at( "height" ).get_to( _config.uHeight );
		if (j.contains( "pos x" )) j.at( "pos x" ).get_to( _config.uPosX );
		if (j.contains( "pos y" )) j.at( "pos y" ).get_to( _config.uPosY );
		if (j.contains( "fullscreen" )) j.at( "fullscreen" ).get_to( _config.bFullscreen );
	}

	void from_json( const nlohmann::json& j, Config& _config )
	{
		if (j.contains( "lastLevel" )) j.at( "lastLevel" ).get_to( _config.sLastLevelActive );
		if (j.contains( "windows" )) j.at( "windows" ).get_to( _config.windows );
	}


	void Config::Load()
	{
		using json = nlohmann::json;
		std::ifstream configFile( Application::Get()->GetRootDirectory() / Config::CONFIG_FILE_NAME, std::ios::in );
		if (configFile.is_open())
		{
			json config;
			configFile >> config;
			config.get_to( *this );
		}
	}

	void Config::Save() const
	{
		using json = nlohmann::json;

		std::ofstream configFile( Application::Get()->GetRootDirectory() / Config::CONFIG_FILE_NAME, std::ios::out );
		if (configFile.is_open())
		{
			json config = *this;

			configFile << config.dump( 1, '\t', true );
		}
	}


	//////////////////////////////////////////////////////////////////////////
	/// Application

	Application* Application::s_pInstance = nullptr;

	Application::Application()
	Application::Application(const StringVec& _Arguments )
	{
		for (const String& sArg : _Arguments)
		{
			if (sArg == "-editor")
				m_bEditorMode = true;
			if (sArg == "-noeditor")
				m_bEditorMode = false;
		}
	}

	Application::~Application()
	{

	}

	bool Application::OnEvent( Event& _event )
	{
		switch (_event.GetEventType())
		{
			case EventType::WindowCloseEvent:
				m_bRunning = false;
				break;

			case EventType::WindowMoveEvent:
			{
				WindowMoveEvent& moveEvent = (WindowMoveEvent&)_event;
				//Log::Info("Window moved to {0} {1}", moveEvent.iNewPosX, moveEvent.iNewPosY);
			}
			break;

			case EventType::WindowSizeEvent:
			{
				WindowSizeEvent& sizeEvent = (WindowSizeEvent&)_event;
				Renderer::OnResize( sizeEvent.iNewWidth, sizeEvent.iNewHeight );
			}
			break;
		}

		return _event.IsHandled();
	}

	void Application::Init()
	{
		ASSERT( s_pInstance == nullptr, "Only one instance of Application authorized" );
		s_pInstance = this;

		Console::Init();
		VERIFY( Log::Init(), "Log did not init properly" );

		PROFILING_INIT();


		EventDispatcher::RegisterListener( this );

		m_Config.pGameConfig = CreateGameConfig();
		m_Config.Load();

		//auto& it = m_Config.windows.find( GetName() );
		//if (it == m_Config.windows.end())
		//	m_Config.windows.   .insert( GetName(), WindowConfig() );
		//
		//m_xWindow = std::unique_ptr<Window>(Window::Create(it->second));
		String sWindowName = GetName();
		m_xWindow = std::unique_ptr<Window>( Window::Create( m_Config.windows[sWindowName] ) );
		m_xWindow->Init();
		m_xWindow->SetVSync( false );

		Input::Init( m_xWindow->GetNativeWindow() );

		Renderer::Init();
		Renderer2D::Init();

		m_timeStep.Advance();
		RegisterAppLayer( &m_ImGuiLayer );
		RegisterAppLayer( &m_EditorLayer );

		auto xLevel = std::make_shared<Plop::LevelBase>();
		xLevel->MakeCurrent();
		m_vecLoadedLevel.push_back( xLevel );
		if (!m_Config.sLastLevelActive.empty())
		{
			if (!xLevel->Load( m_Config.sLastLevelActive ))
			{
				m_Config.sLastLevelActive.clear();
			}

		}

		m_Config.Save();
	}

	void Application::Destroy()
	{
		UnregisterAppLayer( &m_EditorLayer );
		UnregisterAppLayer( &m_ImGuiLayer );

		EventDispatcher::UnregisterListener( this );

		EventDispatcher::Destroy();

		PROFILING_SHUTDOWN();
	}

	void Application::Close()
	{
		m_bRunning = false;
	}

	void Application::Run()
	{

		while (m_bRunning)
		{
			PROFILING_FRAME( "MainThread" );

			Debug::NewFrame();

			m_timeStep.Advance();

			Input::Update( m_timeStep );
			m_xWindow->Update( m_timeStep );


			Renderer2D::NewFrame();
			m_ImGuiLayer.NewFrame();

			auto xLevel = LevelBase::GetCurrentLevel().lock();
			if (xLevel)
				xLevel->BeforeUpdate();

			for (ApplicationLayer* pAppLayer : m_vecAppLayers)
			{
				pAppLayer->OnUpdate( m_timeStep );
			}

			if (xLevel)
			{
				xLevel->Update( m_timeStep );
				xLevel->AfterUpdate();
			}

			m_ImGuiLayer.EndFrame();
			Renderer2D::EndFrame();

			m_xWindow->SwapBuffers();
			Debug::EndFrame();

			Log::FlushFile();
		}

		m_xWindow->Destroy();
	}

	void Application::RegisterAppLayer( ApplicationLayer* _pLayer )
	{
		m_vecAppLayers.push_back( _pLayer );
		std::sort( m_vecAppLayers.begin(), m_vecAppLayers.end(), _SortAppLayer );

		_pLayer->OnRegistered();
	}

	void Application::UnregisterAppLayer( ApplicationLayer* _pLayer )
	{
		for (int i = 0; i < m_vecAppLayers.size(); ++i)
		{
			if (m_vecAppLayers[i] == _pLayer)
			{
				m_vecAppLayers[i] = m_vecAppLayers.back();
				m_vecAppLayers.pop_back();
			}
		}
		std::sort( m_vecAppLayers.begin(), m_vecAppLayers.end(), _SortAppLayer );

		_pLayer->OnUnregistered();
	}


	LevelBasePtr Application::CreateNewLevel()
	{
		LevelBasePtr xLevel = CreateNewLevelPrivate();
		m_vecLoadedLevel.push_back( xLevel );
		return xLevel;
	}

	GameConfig* Application::CreateGameConfig()
	{
		return NEW GameConfig();
	}

	LevelBasePtr Application::CreateNewLevelPrivate()
	{
		LevelBasePtr xLevel = std::make_shared<LevelBase>();
		return xLevel;
	}

}




int main( int argc, char** argv )
{
	Plop::Application* pApp = Plop::CreateApplication();

	StringVec vecArguments;
	vecArguments.reserve( argc - 1 );
	for (int i = 1; i < argc; ++i)
	{
		vecArguments.push_back( argv[i] );
	}

	Plop::Application* pApp = Plop::CreateApplication( vecArguments );
	pApp->Init();
	pApp->Run();
}