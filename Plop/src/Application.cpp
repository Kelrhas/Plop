#include <Plop_pch.h>
#include <PlopInclude.h>

namespace Plop
{
	Application::Application()
	{
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_bRunning)
		{

		}
	}
}




int main(int argc, char** argv)
{
	std::cout << "plop" << std::endl;

	Plop::Application* pApp = Plop::CreateApplication();
	pApp->Run();
}