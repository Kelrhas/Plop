#include "Plop_pch.h"
#include "PlopInclude.h"

#include <fstream>

#include <json.hpp>

#include "Assets/SoundLoader.h"
#include "Audio/AudioManager.h"
#include "Input/Input.h"
#include "Debug/Debug.h"
#include "Debug/Log.h"
#include "Editor/Console.h"
#include "Events/EventDispatcher.h"
#include "Events/WindowEvent.h"
#include "ECS/ComponentManager.h"
#include "ECS/Components/ComponentDefinition.h"
#include "ECS/Components/ComponentIncludes.h"
#include "Renderer/Renderer.h"

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
		std::ifstream configFile(Config::CONFIG_FILE_NAME, std::ios::in);
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

		std::ofstream configFile(Config::CONFIG_FILE_NAME, std::ios::out);
		if (configFile.is_open())
		{
			json config = *this;

			configFile << config.dump( 1, '\t', true );
		}
	}


	//////////////////////////////////////////////////////////////////////////
	/// Application

	Application* Application::s_pInstance = nullptr;

	Application::Application(const StringVec& _Arguments )
	{
		for (const String& sArg : _Arguments)
		{
#ifndef _MASTER
			if (sArg == "-noeditor")
			{
				m_bEditorMode = false;
				m_bShowSceneGraph = false;
			}
			if (sArg == "-noaudio")
			{
				m_bUseAudio = false;
			}
#endif
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
		ASSERTM(s_pInstance == nullptr, "Only one instance of Application authorized");
		s_pInstance = this;

		std::filesystem::current_path(GetRootDirectory());

#ifdef USE_CONSTANT_RANDOM
		RandomSeed(42llu);
#else
		RandomSeed(time(0));
#endif

		Console::Init();
		VERIFYM( Log::Init(), "Log did not init properly" );

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
		m_xWindow->SetVSync( true );

		Input::Init( m_xWindow->GetNativeWindow() );

		if (m_bUseAudio)
		{
			g_pAudioManager = new AudioManager();
			VERIFYM(g_pAudioManager->Init(), "Audio manager did not init properly");
		}

		Renderer::Init();

		m_timeStep.Advance();
		RegisterAppLayer( &m_ImGuiLayer );
		if constexpr (USE_EDITOR)
		{
			RegisterAppLayer(&m_EditorLayer);
		}

		RegisterMandatoryComponents();
		RegisterComponents();

		LevelBasePtr xLevel = CreateNewLevel();
		xLevel->Init();
		if (m_bEditorMode)
			m_EditorLayer.m_xEditingLevel = xLevel;
		else
		{
			m_EditorLayer.m_eLevelState = EditorLayer::LevelState::RUNNING;
			m_xLoadedLevel = xLevel;
		}

		if (!m_Config.sLastLevelActive.empty())
		{
			if (xLevel->Load( m_Config.sLastLevelActive ))
			{
				m_EditorLayer.m_sCurrentLevel = m_Config.sLastLevelActive;
				m_xWindow->SetTitleSuffix(m_EditorLayer.m_sCurrentLevel.stem().string());
			}
			else
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

		if(g_pAudioManager)
			VERIFY( g_pAudioManager->Shutdown() );

		AssetLoader::ClearSoundCache();

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

			Renderer::NewFrame();

			auto xLevel = Application::GetCurrentLevel().lock();
			if (xLevel)
				xLevel->BeforeUpdate();

			
			m_ImGuiLayer.NewFrame();
			for (ApplicationLayer* pAppLayer : m_vecAppLayers)
			{
				pAppLayer->OnUpdate( m_timeStep );
			}

			if (xLevel)
				xLevel->AfterUpdate();

			Renderer::Clear();
			for (ApplicationLayer* pAppLayer : m_vecAppLayers)
			{
				if (pAppLayer == &m_EditorLayer && !m_bEditorMode)
					continue;
				pAppLayer->OnImGuiRender( m_timeStep );
			}

			if constexpr (USE_EDITOR)
			{
				if (!m_bEditorMode)
				{
					if (m_bShowSceneGraph)
						m_EditorLayer.ShowSceneGraph();
				}
			}

			// TODO set docking to bottom
			Console::Draw();

			m_ImGuiLayer.EndFrame();


			Renderer::EndFrame();

			m_xWindow->SwapBuffers();
			Debug::EndFrame();

			Log::FlushFile();

			if (m_EditorLayer.m_eLevelState == EditorLayer::LevelState::STARTING)
			{
				// TODO make async
				if (m_xLoadedLevel == nullptr)
				{
					m_xLoadedLevel = Application::Get()->CreateNewLevel();
					m_xLoadedLevel->Init();
				}

				m_xLoadedLevel->CopyFrom( m_EditorLayer.m_xEditingLevel );
				m_xLoadedLevel->StartFromEditor();

				m_EditorLayer.m_eLevelState = EditorLayer::LevelState::RUNNING;
				m_EditorLayer.UpdateEntityInfo();
			}
			else if (m_EditorLayer.m_eLevelState == EditorLayer::LevelState::STOPPING)
			{
				// TODO make async
				m_xLoadedLevel->StopToEditor();

				m_EditorLayer.m_eLevelState = EditorLayer::LevelState::EDITING;
			}

			if (m_EditorLayer.m_eLevelState == EditorLayer::LevelState::EDITING)
			{
				if (Input::IsMouseLeftPressed() && m_EditorLayer.m_bAllowPicking)
				{
					const glm::vec2 &vWindowPos = Input::GetCursorWindowPos();
					glm::vec2 vViewportPos = m_EditorLayer.GetViewportPosFromWindowPos(vWindowPos, true);
					// TODO viewport panel
					vViewportPos *= m_EditorLayer.m_vViewportPosMaxWindowSpace - m_EditorLayer.m_vViewportPosMinWindowSpace;
#ifdef USE_ENTITY_HANDLE
					m_EditorLayer.m_SelectedEntity = Entity((entt::entity)Renderer::GetEntityId(vViewportPos), Application::GetCurrentLevel().lock()->GetEntityRegistry());
#else
					m_EditorLayer.m_SelectedEntity = Entity((entt::entity)Renderer::GetEntityId(vViewportPos), Application::GetCurrentLevel());
#endif
				}
			}
		}

		m_xWindow->Destroy();

		Destroy();
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
		return xLevel;
	}

	//static
	LevelBaseWeakPtr Application::GetCurrentLevel()
	{
		if (s_pInstance != nullptr)
		{
			if (s_pInstance->m_bEditorMode)
			{
				switch (s_pInstance->m_EditorLayer.m_eLevelState)
				{
					case EditorLayer::LevelState::EDITING:
						return s_pInstance->m_EditorLayer.m_xEditingLevel;

					case EditorLayer::LevelState::STARTING:
					case EditorLayer::LevelState::RUNNING:
					case EditorLayer::LevelState::PAUSED:
						return s_pInstance->m_xLoadedLevel;

					case EditorLayer::LevelState::STOPPING:
						ASSERTM(false, "Access to current level during stop is forbidden");
					default:
						break;
				}
			}
			else
			{
				return s_pInstance->m_xLoadedLevel;
			}
		}

		return LevelBaseWeakPtr();
	}

	bool Application::IsUsingEditorCamera() const
	{
		if(m_bEditorMode)
			return m_EditorLayer.m_eLevelState == EditorLayer::LevelState::EDITING || m_EditorLayer.m_eLevelState == EditorLayer::LevelState::PAUSED;

		return FALSE;
	}

	void Application::ToggleSceneGraph()
	{
		if (!m_bEditorMode)
			m_bShowSceneGraph = !m_bShowSceneGraph;
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

	void Application::RegisterMandatoryComponents()
	{
		//RegisterComponent<Component_Transform, HasEditorUI<Component_Transform>::value>( "Transform" );

#define MACRO_COMPONENT(comp) ComponentManager::RegisterComponent<Component_##comp>( #comp );
	#include "ECS/Components/ComponentList.h"
#undef MACRO_COMPONENT
	}

}




int main( int argc, char** argv )
{

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