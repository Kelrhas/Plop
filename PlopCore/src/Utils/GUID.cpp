#include "GUID.h"

#include "Utils/Random.h"

namespace Plop
{
	const GUID GUID::INVALID = (GUID::base_t)0;

	GUID::GUID()
	{
		m_uID = (uint64_t)RandomInt32() | (((uint64_t)RandomInt32()) << 32llu);
	}
}
