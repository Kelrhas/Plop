#pragma once

#define FMT_HEADER_ONLY
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/format-inl.h>
#include <fmt/color.h>

#include <Editor/Console.h>

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
			uint32_t uColorRGBA = (((uint32_t)color) << 8) | 0x000000FF;
			Console::AddOutput( s, uColorRGBA, LogEntry::Type::Info );
		}
		template<typename ... Args>
		static void Assert	(const char* _pStr, Args&&..._args)
		{
			String str = "Assert:\t";
			str.append( _pStr );

			std::string s = fmt::format( str.c_str(), _args... );
			fmt::v7::color color = fmt::color::purple;

			fmt::print( fg( color ), "{}\n", s );
			uint32_t uColorRGBA = (((uint32_t)color) << 8) | 0x000000FF;
			Console::AddOutput( s, uColorRGBA, LogEntry::Type::Assert );
		}
		template<typename ... Args>
		static void Warn	(const char* _pStr, Args&&..._args)
		{
			String str = "Warn:\t";
			str.append( _pStr );

			std::string s = fmt::format( str.c_str(), _args... );
			fmt::v7::color color = fmt::color::orange;

			fmt::print( fg( color ), "{}\n", s );
			uint32_t uColorRGBA = (((uint32_t)color) << 8) | 0x000000FF;
			Console::AddOutput( s, uColorRGBA, LogEntry::Type::Warning );
		}
		template<typename ... Args>
		static void Error	(const char* _pStr, Args&&..._args)
		{
			String str = "Error:\t";
			str.append( _pStr );

			std::string s = fmt::format( str.c_str(), _args... );
			fmt::v7::color color = fmt::color::red;

			fmt::print( fg( color ), "{}\n", s );
			uint32_t uColorRGBA = (((uint32_t)color) << 8) | 0x000000FF;
			Console::AddOutput( s, uColorRGBA, LogEntry::Type::Error );
		}

		static void Flush	();

	private:
	};
}

