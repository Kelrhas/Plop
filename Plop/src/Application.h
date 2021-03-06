#pragma once

#include <unordered_map>

#include <Window.h>
#include <ApplicationLayer.h>
#include <Debug/ImGuiLayer.h>
#include <Editor/EditorLayer.h>

#include <Events/IEventListener.h>
#include <TimeStep.h>

namespace Plop
{
	struct GameConfig
	{
		// to override
	};

	struct Config
	{
		static const char* CONFIG_FILE_NAME;

		std::unordered_map<String, WindowConfig> windows;
		String sLastLevelActive;

		GameConfig* pGameConfig = nullptr;


		void		Load();
		void		Save() const;
	};


#define REGISTER_COMPONENT(comp) RegisterComponent<comp##Component, true>( #comp )
#define REGISTER_COMPONENT_NO_EDITOR(comp) RegisterComponent<comp##Component, false>( #comp )

	class Application : public IEventListener
	{
	public:
		Application( const StringVec& _Arguments );
		Application( const Application& ) = delete;
		Application( Application&& ) = delete;
		virtual ~Application();

		// IEventListener
		virtual bool			OnEvent( Event& _event ) override;

		virtual	void			Init();
		virtual void			Destroy();
				void			Close(); // user asked

				void			Run();
		virtual String			GetName() const = 0;
		virtual StringPath		GetRootDirectory() const = 0;

				Window&			GetWindow() const { return *m_xWindow; }

				// ApplicationLayer
				void			RegisterAppLayer(ApplicationLayer* _pLayer);
				void			UnregisterAppLayer(ApplicationLayer* _pLayer);


				// Level
				LevelBasePtr	CreateNewLevel();

				// Editor
				bool			IsUsingEditorCamera() const;
				EditorLayer&	GetEditor() { return m_EditorLayer; }
				const EditorLayer&	GetEditor() const { return m_EditorLayer; }


		static Application*		Get() { return s_pInstance; }
		static Config&			GetConfig() { return s_pInstance->m_Config; }
		static TimeStep&		GetTimeStep() { return s_pInstance->m_timeStep; }


	protected:
		template<typename Comp, bool RegisterEditor = true>
				void			RegisterComponent( const char* _pName )
				{
					m_EditorLayer.RegisterComponent<Comp, RegisterEditor>( _pName );
				}


	private:

		virtual GameConfig*		CreateGameConfig();
		virtual LevelBasePtr	CreateNewLevelPrivate();
				void			RegisterMandatoryComponents();
		virtual void			RegisterComponents() {}


		static Application*		s_pInstance;
				Config			m_Config;
				std::unique_ptr<Window> m_xWindow;
				TimeStep		m_timeStep;
				bool			m_bRunning = true;
#ifdef _MASTER
				bool			m_bEditorMode = false;
#else
				bool			m_bEditorMode = true;
#endif

				std::vector<ApplicationLayer*> m_vecAppLayers;
				ImGuiLayer		m_ImGuiLayer;
				EditorLayer		m_EditorLayer;
		static bool				_SortAppLayer(const ApplicationLayer* _pLayerA, const ApplicationLayer* _pLayerB)
		{
			return _pLayerA->GetPriority() < _pLayerB->GetPriority();
		}

				std::vector<LevelBasePtr>	m_vecLoadedLevel;
	};


	Application* CreateApplication( const StringVec& _Arguments );
}

