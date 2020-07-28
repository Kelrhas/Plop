#include <Plop_pch.h>
#include <PlopInclude.h>

#include <fstream>

#include <json.hpp>

#include <Log.h>
#include <Events/EventDispatcher.h>

namespace Plop
{
	///
	/// Config
	///
	const char* Config::CONFIG_FILE_NAME = "Config.json";

	void to_json(nlohmann::json& j, const WindowConfig& _config)
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

	void to_json(nlohmann::json& j, const Config& _config)
	{
		j = nlohmann::json
		{
			{"windows", _config.windows}
		};
	}

	void from_json(const nlohmann::json& j, WindowConfig& _config)
	{
		if (j.contains("width")) j.at("width").get_to(_config.uWidth);
		if (j.contains("height")) j.at("height").get_to(_config.uHeight);
		if (j.contains("pos x")) j.at("pos x").get_to(_config.uPosX);
		if (j.contains("pos y")) j.at("pos y").get_to(_config.uPosY);
		if (j.contains("fullscreen")) j.at("fullscreen").get_to(_config.bFullscreen);
	}

	void from_json(const nlohmann::json& j, Config& _config)
	{
		if (j.contains("windows")) j.at("windows").get_to(_config.windows);
	}


	void Config::Load()
	{
		using json = nlohmann::json;
		std::ifstream configFile(Config::CONFIG_FILE_NAME, std::ios::in);
		if (configFile.is_open())
		{
			json config;
			configFile >> config;
			config.get_to(*this);
		}
	}

	void Config::Save() const
	{
		using json = nlohmann::json;

		std::ofstream configFile(Config::CONFIG_FILE_NAME, std::ios::out);
		if (configFile.is_open())
		{
			json config = *this;
			
			configFile << config.dump(1, '\t', true);
		}
	}


	///
	/// Application
	///

	Application* Application::s_pInstance = nullptr;

	Application::Application()
	{
		Init();
	}

	Application::~Application()
	{

	}

	bool Application::OnEvent( Event& _event )
	{
		if (_event.GetEventType() == EventType::WindowCloseEvent)
			m_bRunning = false;

		return _event.IsHandled();
	}

	void Application::Init()
	{
		VERIFY(Log::Init());

		ASSERT( s_pInstance == nullptr, "Only one instance of Application authorized" );

		s_pInstance = this;

		EventDispatcher::RegisterListener( this );

		m_Config.pGameConfig = CreateGameConfig();
		m_Config.Load();

		m_xWindow = std::unique_ptr<Window>(Window::Create(m_Config.windows["Test"]));
		m_xWindow->Init();

		m_Config.Save();
	}

	void Application::Destroy()
	{
		EventDispatcher::UnregisterListener( this );

		EventDispatcher::Destroy();
	}

	void Application::Run()
	{

		while (m_bRunning)
		{
			m_xWindow->Update();
		}

		m_xWindow->Destroy();
	}

	GameConfig* Application::CreateGameConfig()
	{
		return new GameConfig();
	}
}




int main(int argc, char** argv)
{
	Plop::Application* pApp = Plop::CreateApplication();
	pApp->Run();
}