#include "Plop_pch.h"
#include "Utils/OSDialogs.h"

#include <windows.h>
#include <commdlg.h>

#include "Application.h"

namespace Plop
{
	// see https://docs.microsoft.com/en-us/windows/win32/shell/common-file-dialog for "newer" API

	const char *Dialog::IMAGE_FILTER = "Image file (png, jpg, tga...)\0*.png;*.jpg;*.jpeg;*.tga;*.psd;*.bmp;*.gif\0";
	const char *Dialog::SOUND_FILTER = "Sound file (wav, mp3, flac...)\0*.wav;*.mp3;*.pcm;*.flac;*.wma\0";
	const char *Dialog::LEVEL_FILTER = "Level file (.lvl)\0*.lvl\0";
	const char *Dialog::SPRITESHEET_FILTER = "Spritesheet definition file (.ssdef)\0*.ssdef\0";
	const char *Dialog::PREFABLIB_FILTER = "Prefab library (.prefablib)\0*.prefablib\0";
	const char *Dialog::PREFAB_FILTER = "Prefab (.prefab)\0*.prefab\0";

	bool Dialog::OpenFile( StringPath& _sFilePath, const char* _pFilter )
	{
		OPENFILENAME ofn;					// common dialog box structure
		TCHAR szFile[260] = { 0 };			// if using TCHAR macros

		ZeroMemory( &ofn, sizeof( ofn ) );
		ofn.lStructSize = sizeof( ofn );
		ofn.hwndOwner = (HWND)Application::Get()->GetWindow().GetNativeWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof( szFile );
		ofn.lpstrFilter = _pFilter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		StringPath rootAbsolute = std::filesystem::current_path();
		String sRootAbs = rootAbsolute.string(); // keep memory alive for lpstrInitialDir
		ofn.lpstrInitialDir = sRootAbs.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON;

		if (GetOpenFileName( &ofn ) == TRUE)
		{
			_sFilePath = ofn.lpstrFile;

			// make relative
			if (_sFilePath.is_absolute())
			{
				StringPath activeDirectory = std::filesystem::current_path();
				StringPath relativePath = _sFilePath.lexically_relative( activeDirectory );

				if (!relativePath.empty())
					_sFilePath = relativePath;
			}

			return true;
		}


		return false;
	}


	bool Dialog::SaveFile( StringPath& _sFilePath, const char* _pFilter )
	{
		OPENFILENAME ofn;					// common dialog box structure
		TCHAR szFile[260] = { 0 };			// if using TCHAR macros

		ZeroMemory( &ofn, sizeof( ofn ) );
		ofn.lStructSize = sizeof( ofn );
		ofn.hwndOwner = (HWND)Application::Get()->GetWindow().GetNativeWindow();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof( szFile );
		ofn.lpstrFilter = _pFilter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		StringPath rootAbsolute = std::filesystem::current_path();
		String sRootAbs = rootAbsolute.string(); // keep memory alive for lpstrInitialDir
		ofn.lpstrInitialDir = sRootAbs.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_DONTADDTORECENT | OFN_ENABLESIZING | OFN_NOCHANGEDIR | OFN_NONETWORKBUTTON | OFN_NOREADONLYRETURN | OFN_OVERWRITEPROMPT;

		if (GetSaveFileName( &ofn ) == TRUE)
		{
			_sFilePath = ofn.lpstrFile;

			// make relative
			if (_sFilePath.is_absolute())
			{
				StringPath activeDirectory = std::filesystem::current_path();
				StringPath relativePath = _sFilePath.lexically_relative( activeDirectory );

				if (!relativePath.empty())
					_sFilePath = relativePath;
			}
			return true;
		}


		return false;
	}
}