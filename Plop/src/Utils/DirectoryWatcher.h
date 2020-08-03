#pragma once

#include <filesystem>
#include <atomic>
#include <functional>
#include <unordered_map>


namespace Plop
{
	class DirectoryWatcher
	{
	public:
		enum FileStatus
		{
			CREATED,
			MODIFIED,
			DELETED
		};

		DirectoryWatcher();
		DirectoryWatcher(const std::filesystem::path& _sPath, float _fTimeIntervalInSeconds = 0.1f);
		~DirectoryWatcher();

		DirectoryWatcher& operator =(const Plop::DirectoryWatcher& _other);

		void Start(const std::function<void(const std::filesystem::path&, FileStatus)>& _callback);
		void Stop();


	private:

		void ThreadRun(const std::function<void(const std::filesystem::path&, FileStatus)>& _callback);

		std::atomic_bool				m_bWatching = false;
		std::filesystem::path			m_sPath;
		float							m_fTimeIntervalInSeconds = 0.1f;
		std::unordered_map<String, std::filesystem::file_time_type> m_mapFiles;

	};
}
