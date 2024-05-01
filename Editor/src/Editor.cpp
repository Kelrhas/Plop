#include "Config.h"

#include "Editor.h"
#include "Debug/Log.h"

#include <iostream>

Plop::Application *Plop::CreateApplication(const StringVec &_Arguments)
{
	return new Editor::EditorApp(_Arguments);
}

namespace Editor
{

	EditorApp::EditorApp(const StringVec &_Arguments)
		: Application(_Arguments)
	{
	}

	void EditorApp::Init()
	{
		Plop::Application::Init();
		Play();
	}

	/*
	void EditorApp::Update()
	{
		std::cout << "Editor update" << std::endl;
		dllWatcher.Update();
		if (m_pGameApp)
			m_pGameApp->Update();

		std::cout << std::endl;
		Sleep(1000);
	}
	*/
	void EditorApp::Play()
	{
		/*
#ifdef _DEBUG
		dllWatcher.Start("../build/bin/SampleLib_d.dll",
#else
		dllWatcher.Start("../build/bin/SampleLib_r.dll",
#endif
			[&](std::string_view _sFile) -> void
			{
				if (m_hLibrary)
				{
					FreeLibrary(m_hLibrary);
					m_hLibrary = NULL;
				}

				// Copy the DLL so we don't hold a lock on the original file.
				std::string newDLLLocation = _sFile.data();
				newDLLLocation += ".reloaded.dll";
				if (CopyFileA(_sFile.data(), newDLLLocation.c_str(), FALSE))
				{
					m_hLibrary = LoadLibrary(newDLLLocation.c_str());
					if (m_hLibrary)
					{
						typedef Plop::Application* (__stdcall* CreateApplicationForEditor_fn)();
						auto fn = GetProcAddress(m_hLibrary, "CreateApplicationForEditor");
						CreateApplicationForEditor_fn func = (CreateApplicationForEditor_fn)fn;
						if (func && !m_pGameApp)
						{
							m_pGameApp = func();
							m_pGameApp->Init();
						}
						std::cout << "Library loaded" << std::endl;
					}
				}
			});
		dllWatcher.Update(); // trigger the first reload
		*/
	}

	void EditorApp::Stop()
	{
		m_pGameApp->Destroy();
		delete m_pGameApp;
		m_pGameApp = nullptr;
		//dllWatcher.Stop();
		
	}
}
