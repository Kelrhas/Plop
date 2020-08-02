#pragma once

namespace Plop
{
	class Log
	{
	public:

		static bool Init	();

		static void Info	(const char* _pStr, ...);
		static void Assert	(const char* _pStr, ...);
		static void Warn	(const char* _pStr, ...);
		static void Error	(const char* _pStr, ...);

		static void Flush	();

	private:
		static void Output	(const char* _pBuff, va_list _args);

		static HANDLE	s_hConsole;
	};
}

