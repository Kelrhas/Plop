#pragma once

#include <fstream>

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/format-inl.h>
#include <fmt/color.h>

#include "Editor/Console.h"

namespace Plop
{
	class Log
	{
	public:

		static bool Init	();

		template<typename ... Args>
		static void Info	(const char* _pStr, Args&&..._args)
		{
			String str = "Info:\t";
			str.append( _pStr );

			std::string s = fmt::format(str.c_str(), _args... );
			fmt::v7::color color = fmt::color::white;

			fmt::print( fg( color ), "{}\n", s );
			Console::AddOutput( s, LogEntry::Type::Info );
			LogToFile( s );
		}
		template<typename ... Args>
		static void Assert	(const char* _pStr, Args&&..._args)
		{
			String str = "Assert:\t";
			str.append( _pStr );

			std::string s = fmt::format( str.c_str(), _args... );
			fmt::v7::color color = fmt::color::purple;

			fmt::print( fg( color ), "{}\n", s );
			Console::AddOutput( s, LogEntry::Type::Assert );
			LogToFile( s );
			FlushFile(); // we flush to be sure that we have the log in the file before a possible crash
		}
		template<typename ... Args>
		static void Warn	(const char* _pStr, Args&&..._args)
		{
			String str = "Warn:\t";
			str.append( _pStr );

			std::string s = fmt::format( str.c_str(), _args... );
			fmt::v7::color color = fmt::color::orange;

			fmt::print( fg( color ), "{}\n", s );
			Console::AddOutput( s, LogEntry::Type::Warning );
			LogToFile( s );
		}
		template<typename ... Args>
		static void Error	(const char* _pStr, Args&&..._args)
		{
			String str = "Error:\t";
			str.append( _pStr );

			std::string s = fmt::format( str.c_str(), _args... );
			fmt::v7::color color = fmt::color::red;

			fmt::print( fg( color ), "{}\n", s );
			Console::AddOutput( s, LogEntry::Type::Error );
			LogToFile( s );
			FlushFile(); // we flush to be sure that we have the log in the file before a possible crash
		}


		static void LogToFile( const String& _str );
		static void FlushFile();

	private:
	};
}

