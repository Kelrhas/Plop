#pragma once

#include "Types.h"

namespace Plop
{
	struct Dialog
	{
		static const char* IMAGE_FILTER;
		static const char* SOUND_FILTER;
		static const char* LEVEL_FILTER;
		static const char* SPRITESHEET_FILTER;
		static const char* PREFABLIB_FILTER;
		static const char* PREFAB_FILTER;


		static bool OpenFile( StringPath& _sFilePath, const char* _pFilter );
		static bool SaveFile( StringPath& _sFilePath, const char* _pFilter );
	};
}
