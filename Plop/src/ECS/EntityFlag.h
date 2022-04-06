#pragma once

#include <Utils/EnumFlags.h>

namespace Plop
{
	enum class EntityFlag
	{
		NONE,
		DYNAMIC_GENERATION, // means no serialization

		COUNT
	};
	using EntityFlagBits = EnumFlags<EntityFlag>;
}