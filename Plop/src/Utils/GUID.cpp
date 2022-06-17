#include "Plop_pch.h"
#include "GUID.h"

#include "Utils/Random.h"

namespace Plop
{
	GUID::GUID()
	{
		m_uID = (uint64_t)RandomInt32() | (((uint64_t)RandomInt32()) << 32llu);
	}

	GUID::GUID(uint64_t _uID) : m_uID(_uID)
	{

	}
}