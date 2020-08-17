#pragma once

#include <functional>
#include <imgui.h>

namespace Plop
{
	struct LogEntry
	{
		enum class Type
		{
			Info,
			Assert,
			Warning,
			Error,
			Command,

		} eType = Type::Info;

		String sLog;
		uint32_t uColor = 0xFFFFFFFF;
	};

	class Console
	{
		static const int MAX_ENTRY = 512;
	public:

		static void					Init();
		static void					AddOutput( const String& _str, uint32_t _uColor = 0xFFFFFFFF, LogEntry::Type _eType = LogEntry::Type::Info);

		static void					RegisterCommand( const char* _pCommand, std::function<void( const String& )> _callback );
		static bool					ExecCommand( const char* _pText );

		static void					Draw();

	private:
		using CommandMap = std::unordered_map<String, std::function<void( const String& )>>;


		static int					InputCallback( ImGuiInputTextCallbackData* _pData );

		static char					s_pInputBuffer[256];
		static CommandMap			s_mapCommands;
		static LogEntry				s_ConsoleBuffer[MAX_ENTRY];
		static uint32_t				s_uEntryIndex;
		static ImGuiTextFilter		s_Filter;
	};
}
