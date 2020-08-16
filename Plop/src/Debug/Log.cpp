#include "Plop_pch.h"
#include "Log.h"

namespace Plop
{
	bool Log::Init()
	{
		Info("Info");
		Assert("Assert");
		Warn("Warn");
		Error("Error");

		return true;
	}

	void Log::Flush()
	{
		// TODO flush the file if we have one still open
	}
}
