#pragma once

#include "Editor/Console.h"
#include "Types.h"

//#define FMT_HEADER_ONLY
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format-inl.h>
#include <fmt/format.h>
#include <fstream>

namespace Plop
{
	class Log
	{
	public:
		static bool Init();

		template<typename... Args>
		static void Info(const char *_pStr, Args &&..._args)
		{
			std::string sMsg = fmt::format(fmt::runtime(_pStr), _args...);
			String		str	 = "Info:\t" + sMsg;

			fmt::color col = fmt::color::white;

			fmt::print(fg(col), "{}\n", str);
			Console::AddOutput(str, LogEntry::Type::Info);
			LogToFile(str);
		}
		template<typename... Args>
		static void Info(const String &_str, Args &&..._args)
		{
			Info(_str.c_str(), _args...);
		}

		template<typename... Args>
		static void Assert(const char *_pStr, Args &&..._args)
		{
			std::string sMsg = fmt::format(fmt::runtime(_pStr), _args...);
			String		str	 = "Assert:\t" + sMsg;

			fmt::color color = fmt::color::purple;

			fmt::print(fg(color), "{}\n", str);
			Console::AddOutput(str, LogEntry::Type::Assert);
			LogToFile(str);
			FlushFile(); // we flush to be sure that we have the log in the file before a possible crash
		}
		//static void Assert(const char *_pStr)
		//{
		//	std::string s	  = fmt::format("Assert:\t{}", _pStr);
		//	fmt::color	color = fmt::color::purple;
		//
		//	fmt::print(fg(color), "{}\n", s);
		//	Console::AddOutput(s, LogEntry::Type::Assert);
		//	LogToFile(s);
		//	FlushFile(); // we flush to be sure that we have the log in the file before a possible crash
		//}
		template<typename... Args>
		static void Assert(const String &_str, Args &&..._args)
		{
			Assert(_str.c_str(), _args...);
		}

		template<typename... Args>
		static void Warn(const char *_pStr, Args &&..._args)
		{
			std::string sMsg = fmt::format(fmt::runtime(_pStr), _args...);

			String str = "Warn:\t" + sMsg;

			fmt::color color = fmt::color::orange;

			fmt::print(fg(color), "{}\n", str);
			Console::AddOutput(str, LogEntry::Type::Warning);
			LogToFile(str);
		}
		template<typename... Args>
		static void Warn(const String &_str, Args &&..._args)
		{
			Warn(_str.c_str(), _args...);
		}

		template<typename... Args>
		static void Error(const char *_pStr, Args &&..._args)
		{
			std::string sMsg = fmt::format(fmt::runtime(_pStr), _args...);

			String str = "Error:\t" + sMsg;

			fmt::color color = fmt::color::red;

			fmt::print(fg(color), "{}\n", str);
			Console::AddOutput(str, LogEntry::Type::Error);
			LogToFile(str);
			FlushFile(); // we flush to be sure that we have the log in the file before a possible crash
		}
		template<typename... Args>
		static void Error(const String &_str, Args &&..._args)
		{
			Error(_str.c_str(), _args...);
		}


		static void LogToFile(const String &_str);
		static void FlushFile();

	private:
	};
} // namespace Plop

namespace fmt
{
	template<glm::length_t L, typename T, glm::qualifier Q>
	struct formatter<glm::vec<L, T, Q>> : formatter<T>
	{
		using Vec = glm::vec<L, T, Q>;

		template<typename FormatContext>
		auto format(const Vec &_v, FormatContext &ctx) -> decltype(ctx.out())
		{
			auto out = std::copy_n("{", 1, ctx.out());
			for (int i = 0; i < L; ++i)
			{
				out = formatter<T>::format(_v[i], ctx);
				if (i < L - 1)
					out = std::copy_n(",", 1, out);
			}
			out = std::copy_n("}", 1, out);

			return out;
		}
	};


	template<glm::length_t Row, glm::length_t Col, typename T, glm::qualifier Q>
	struct formatter<glm::mat<Row, Col, T, Q>> : formatter<typename glm::mat<Row, Col, T, Q>::col_type>
	{
		using Mat = glm::mat<Row, Col, T, Q>;

		template<typename FormatContext>
		auto format(const Mat &_m, FormatContext &ctx) -> decltype(ctx.out())
		{
			auto out = std::copy_n("{", 1, ctx.out());
			for (int i = 0; i < Row; ++i)
			{
				formatter<typename Mat::col_type>::format(_m[i], ctx);
				if (i < Row - 1)
					out = std::copy_n(",", 1, out);
			}
			out = std::copy_n("}", 1, out);

			return out;
		}
	};
} // namespace fmt