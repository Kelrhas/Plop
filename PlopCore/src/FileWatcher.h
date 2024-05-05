#pragma once

#include <Windows.h>
#include <filesystem>
#include <functional>
#include <string_view>

namespace Plop
{
	class FileWatcher
	{
		using Callback_t = void(std::string_view);

	public:
		bool Start(std::string_view _sFile, std::function<Callback_t> _fnCallback);
		void Update();
		void Stop();

	private:
		[[nodiscard]] bool Check();

		std::filesystem::path	  m_sFile;
		std::function<Callback_t> m_fnCallback;
		//uint64_t				  m_uLastFileTime = 0;
		std::filesystem::file_time_type m_uLastFileTime;
	};
} // namespace Plop
