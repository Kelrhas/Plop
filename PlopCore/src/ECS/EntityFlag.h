#pragma once

#include <Utils/EnumFlags.h>

namespace Plop
{
	enum class EntityFlag
	{
		NONE,
		NO_SERIALISATION,					// means no serialization
		HIDE,								// do not render this entity (does not take care of children)

		COUNT,

		// shortcuts & aliases
		DYNAMIC_GENERATION = NO_SERIALISATION,
	};
	using EntityFlagBits = EnumFlags<EntityFlag>;
}