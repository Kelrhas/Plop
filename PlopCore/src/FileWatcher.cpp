#include "Config.h"

#include "FileWatcher.h"
#include "Debug/Log.h"

#include <iostream>

using namespace Plop;

/// <summary>
/// Watches the file for any change
/// </summary>
/// <param name="_sFile">Path to the file</param>
/// <param name="_fnCallback">Callback function to be called on change</param>
/// <returns>Whther the file is being watched, false if no file exists at that location</returns>
bool FileWatcher::Start(std::string_view _sFile, std::function<Callback_t> _fnCallback)
{
	if (std::filesystem::exists(_sFile))
	{
		m_sFile		 = _sFile;
		m_fnCallback = std::move(_fnCallback);

		Log::Info("Start watching file {}", m_sFile.string());
		return true;
	}
	else
		Log::Error("No file found at {}", (std::filesystem::current_path() / _sFile).string());

	return false;
}

void FileWatcher::Update()
{
	if (m_fnCallback && !m_sFile.empty())
	{
		if (Check())
		{
			m_fnCallback(m_sFile.string());
		}
	}
}

void FileWatcher::Stop()
{
	m_sFile.clear();
	m_fnCallback	= nullptr;
	m_uLastFileTime = {};
}

bool FileWatcher::Check()
{
	bool bHasChanged = false;

#if 0
	#ifdef _WINDOWS
	HANDLE hFile = CreateFileA(m_sFile.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile)
	{
		uint64_t uFileTime;
		if (GetFileTime(hFile, NULL, NULL, (FILETIME *)&uFileTime))
		{
			bHasChanged		= m_uLastFileTime < uFileTime;
			m_uLastFileTime = uFileTime;
		}

		CloseHandle(hFile);
	}
	#endif
#else
	auto fileTime = std::filesystem::last_write_time(m_sFile);
	if (fileTime > m_uLastFileTime)
	{
		bHasChanged		= true;
		m_uLastFileTime = fileTime;
	}

#endif

	return bHasChanged;
}