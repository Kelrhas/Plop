#include "Utils.h"

#pragma comment(lib, "rpcrt4.lib") // for UUID and such

namespace Plop::Utils
{
	bool Check4CharCode( uint8_t _char[4], const char* _code )
	{
		return _char[0] == _code[0] && _char[1] == _code[1] && _char[2] == _code[2] && _char[3] == _code[3];
	}


#ifdef PLATFORM_WINDOWS
	UUID GenerateUUID()
	{
		UUID uuid;
		::CoCreateGuid(&uuid);
		return uuid;
	}

	UUID GUIDFromString(const String &_s)
	{
		UUID uuid;
		::UuidFromStringA((RPC_CSTR)_s.c_str(), &uuid);
		return uuid;
	}

	String UUIDToString(const UUID &_uuid)
	{
		RPC_CSTR str;
		UUID uuid = _uuid;
		::UuidToStringA(&uuid, &str);

		String s = (char *)str;
		::RpcStringFreeA(&str);
		return s;
	}
#endif
}