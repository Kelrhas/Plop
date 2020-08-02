#include "Plop_pch.h"
#include "Log.h"

namespace Plop
{
	HANDLE Log::s_hConsole = NULL;

	bool Log::Init()
	{
		s_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		if (s_hConsole == INVALID_HANDLE_VALUE) {
			return false;
		}

		DWORD dwMode = 0;
		if (!GetConsoleMode(s_hConsole, &dwMode)) {
			return false;
		}
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(s_hConsole, dwMode)) {
			return false;
		}

		Info("Info");
		Assert("Assert");
		Warn("Warn");
		Error("Error");

		return true;
	}

	void Log::Info(const char* _pStr, ...)
	{
		String str = "Info:\t";
		str.append(_pStr);

		SetConsoleTextAttribute(s_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

		va_list args;
		va_start(args, _pStr);

		Output(str.c_str(), args);

		va_end(args);
	}

	void Log::Assert(const char* _pStr, ...)
	{
		String str = "Assert:\t";
		str.append(_pStr);

		SetConsoleTextAttribute(s_hConsole, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);

		va_list args;
		va_start(args, _pStr);

		Output(str.c_str(), args);

		va_end(args);
	}

	void Log::Warn(const char* _pStr, ...)
	{
		String str = "Warn:\t";
		str.append(_pStr);

		SetConsoleTextAttribute(s_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

		va_list args;
		va_start(args, _pStr);

		Output(str.c_str(), args);

		va_end(args);
	}

	void Log::Error(const char* _pStr, ...)
	{
		String str = "Error:\t";
		str.append(_pStr);

		SetConsoleTextAttribute(s_hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);

		va_list args;
		va_start(args, _pStr);

		Output(str.c_str(), args);

		va_end(args);
	}

	void Log::Flush()
	{
		// TODO flush the file if we have one still open
	}

	void Log::Output(const char* _pBuff, va_list _args)
	{
		char buf[2048] = "";

		vsnprintf(buf, 2048, _pBuff, _args);

		std::cout << buf << std::endl;
	}
}
