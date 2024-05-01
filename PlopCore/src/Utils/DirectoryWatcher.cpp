#include "DirectoryWatcher.h"

#include "Debug/Debug.h"

#include <thread>

namespace Plop
{
	DirectoryWatcher::DirectoryWatcher()
	{

	}

	DirectoryWatcher::DirectoryWatcher(const std::filesystem::path& _sPath, float _fTimeIntervalInSeconds /*= 0.1f*/)
		: m_sPath(_sPath), m_fTimeIntervalInSeconds(_fTimeIntervalInSeconds)
	{

	}


	DirectoryWatcher::~DirectoryWatcher()
	{
		Stop();
	}

	DirectoryWatcher& DirectoryWatcher::operator =(const Plop::DirectoryWatcher& _other)
	{
		// do not auto start
		m_bWatching = false;
		m_sPath = _other.m_sPath;
		m_fTimeIntervalInSeconds = _other.m_fTimeIntervalInSeconds;
		m_mapFiles.clear();

		return *this;
	}

	void DirectoryWatcher::Start(const std::function<void(const std::filesystem::path&, FileStatus)>& _callback)
	{
		ASSERTM(!m_bWatching, "Watcher is already watching, please call Stop before");
		if (m_bWatching)
			return;

		ASSERTM(!m_sPath.empty(), "Watcher has no directory to watch");
		if (m_sPath.empty())
			return;

		// init the files
		m_mapFiles.clear();
		for (auto& file : std::filesystem::recursive_directory_iterator(m_sPath))
		{
			m_mapFiles[file.path().string()] = std::filesystem::last_write_time(file.path());
		}

		m_bWatching = true;

		std::thread watchingThread = std::thread(&DirectoryWatcher::ThreadRun, this, _callback);
		watchingThread.detach();
	}
		
	void DirectoryWatcher::Stop()
	{
		m_bWatching = false;
	}

	void DirectoryWatcher::ThreadRun(const std::function<void(const std::filesystem::path&, FileStatus)>& _callback)
	{
		while (m_bWatching)
		{
			auto itSavedFile = m_mapFiles.begin();
			while (itSavedFile != m_mapFiles.end())
			{
				if (!std::filesystem::exists(itSavedFile->first))
				{
					itSavedFile = m_mapFiles.erase(itSavedFile);
					_callback(itSavedFile->first, DELETED);
				}
				else
					++itSavedFile;
			}

			for (auto& file : std::filesystem::recursive_directory_iterator(m_sPath))
			{
				auto it = m_mapFiles.find(file.path().string());
				if (it != m_mapFiles.end())
				{
					std::filesystem::file_time_type lastWrite = std::filesystem::last_write_time(it->first);
					if (lastWrite != it->second)
					{
						it->second = lastWrite;
						_callback(file.path(), MODIFIED);
					}
				}
				else
				{
					m_mapFiles[file.path().string()] = std::filesystem::last_write_time(file.path());
					_callback(file.path(), CREATED);
				}
			}
		}
	}
}