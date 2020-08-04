#pragma once

#include <unordered_map>

#include <Window.h>
#include <ApplicationLayer.h>
#include <Debug/ImGuiLayer.h>

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

		GameConfig* pGameConfig = nullptr;


		void		Load();
		void		Save() const;
	};

	class Application : public IEventListener
	{
	public:
		Application();
		virtual ~Application();

		// IEventListener
		virtual bool			OnEvent( Event& _event ) override;

				void			Init();
				void			Destroy();

				void			Run();

		virtual GameConfig*		CreateGameConfig();

				Window&			GetWindow() const { return *m_xWindow; }

				// ApplicationLayer
				void			RegisterAppLayer(ApplicationLayer* _pLayer);
				void			UnregisterAppLayer(ApplicationLayer* _pLayer);


		static Application*		Get() { return s_pInstance; }
		static Config&			GetConfig() { return s_pInstance->m_Config; }

	private:
		static Application*		s_pInstance;

				Config			m_Config;
				std::unique_ptr<Window> m_xWindow;
				TimeStep		m_timeStep;
				bool			m_bRunning = true;

				std::vector<ApplicationLayer*> m_vecAppLayers;
				ImGuiLayer		m_ImGuiLayer;
		static bool				_SortAppLayer(const ApplicationLayer* _pLayerA, const ApplicationLayer* _pLayerB)
		{
			return _pLayerA->GetPriority() < _pLayerB->GetPriority();
		}
	};


	Application* CreateApplication();
}

