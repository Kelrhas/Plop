#pragma once

#include "Application.h"
// #include "FileWatcher.h"

#include <chrono>
#include <Windows.h>

#ifdef _MASTER
#error Editor is not meant to be used in _MASTER
#endif

namespace Editor
{
	class EditorApp : public Plop::Application
	{
	public:
		EditorApp(const StringVec &_Arguments);
		EditorApp(const EditorApp &) = delete;
		EditorApp( EditorApp&& ) = delete;

		virtual	void				Init() override;
		//virtual void				Destroy();
		//		void				Close(); // user asked

		virtual String				GetName() const override { return "Editor"; }

				void				Play();
				void				Stop();

	private:
		virtual StringPath			GetRootDirectory() const override { return StringPath("Editor"); }


		Plop::Application *m_pGameApp = nullptr;
		//Plop::FileWatcher	dllWatcher;
		HMODULE m_hLibrary = NULL;
	};
}
