#pragma once

#include "PlopInclude.h"
#include <Window.h>

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

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Init();
		void Reset();

		void Run();


		virtual GameConfig* CreateGameConfig();
		static Config& GetConfig() { return s_pInstance->m_Config; }

	private:
		static Application* s_pInstance;

		Config m_Config;
		std::unique_ptr<Window> m_xWindow;
		bool m_bRunning = true;
	};


	Application* CreateApplication();
}

