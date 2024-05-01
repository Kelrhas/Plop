#pragma once

#include "Types.h"

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#endif

namespace Plop::Utils
{
	bool	Check4CharCode( uint8_t _char[4], const char* _code );


#ifdef PLATFORM_WINDOWS
	UUID	GenerateUUID();
	UUID	UUIDFromString(const String &_s);
	String	UUIDToString(const UUID &_uuid);
#endif
}