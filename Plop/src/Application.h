#pragma once

#include "PlopInclude.h"

namespace Plop
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		bool m_bRunning = true;
	};


	Application* CreateApplication();
}

