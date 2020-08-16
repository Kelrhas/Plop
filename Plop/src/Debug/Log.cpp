#include "Plop_pch.h"
#include "Log.h"

namespace Plop
{
	bool Log::Init()
	{
		Info("Info\n");
		Assert("Assert\n");
		Warn("Warn\n");
		Error("Error\n");

		return true;
	}

	//template<typename ... Args>
	//void Log::Info(const char* _pStr, Args&&..._args)
	//{
	//	String str = "Info:\t";
	//	str.append(_pStr);

	//	fmt::print( fg(fmt::color::white), str.c_str(), _args...);
	//}

	//template<typename ... Args>
	//void Log::Assert(const char* _pStr, Args&&..._args )
	//{
	//	String str = "Assert:\t";
	//	str.append(_pStr);

	//	fmt::print( fg( fmt::color::purple ), str.c_str(), _args... );
	//}

	//template<typename ... Args>
	//void Log::Warn(const char* _pStr, Args&&..._args )
	//{
	//	String str = "Warn:\t";
	//	str.append(_pStr);

	//	fmt::print( fg( fmt::color::orange ), str.c_str(), _args... );
	//}

	//template<typename ... Args>
	//void Log::Error(const char* _pStr, Args&&..._args )
	//{
	//	String str = "Error:\t";
	//	str.append(_pStr);
	//	str.append( "\n" );

	//	fmt::print( fg( fmt::color::red ), str.c_str(), _args... );
	//}

	void Log::Flush()
	{
		// TODO flush the file if we have one still open
	}
}
