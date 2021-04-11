#include "Plop_pch.h"
#include "Utils.h"


namespace Plop::Utils
{
	bool Check4CharCode( uint8_t _char[4], const char* _code )
	{
		return _char[0] == _code[0] && _char[1] == _code[1] && _char[2] == _code[2] && _char[3] == _code[3];
	}
}