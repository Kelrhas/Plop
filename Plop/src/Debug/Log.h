#pragma once

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

			fmt::print( fg( fmt::color::white ), str.c_str(), _args... );
		}
		template<typename ... Args>
		static void Assert	(const char* _pStr, Args&&..._args)
		{
			String str = "Assert:\t";
			str.append( _pStr );

			fmt::print( fg( fmt::color::purple ), str.c_str(), _args... );
		}
		template<typename ... Args>
		static void Warn	(const char* _pStr, Args&&..._args)
		{
			String str = "Warn:\t";
			str.append( _pStr );

			fmt::print( fg( fmt::color::orange ), str.c_str(), _args... );
		}
		template<typename ... Args>
		static void Error	(const char* _pStr, Args&&..._args)
		{
			String str = "Error:\t";
			str.append( _pStr );
			str.append( "\n" );

			fmt::print( fg( fmt::color::red ), str.c_str(), _args... );
		}

		static void Flush	();

	private:
	};
}

