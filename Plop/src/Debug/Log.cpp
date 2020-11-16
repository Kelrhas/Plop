#include "Plop_pch.h"
#include "Log.h"

namespace Plop
{
	const char* LOG_FILE = "log.txt";
	std::ofstream s_LogFile;

	bool Log::Init()
	{
		s_LogFile.open( LOG_FILE, std::ios::out );

		Info("Info");
		Assert("Assert");
		Warn("Warn");
		Error("Error");

		return true;
	}


	void Log::LogToFile( const String& _str )
	{
		if (s_LogFile)
		{
			s_LogFile << _str << std::endl;
		}
	}

	void Log::FlushFile()
	{
		// flush the file if we have one still open
		if (s_LogFile)
			s_LogFile.flush();
	}
}
