#pragma once

#include "Renderer/Spritesheet.h"

namespace Plop::AssetLoader
{
	SpritesheetHandle GetSpritesheet( const StringPath& _sFilepath );

	void ClearSpritesheetCache();

	SpritesheetHandle PickSpritesheetFromCache();
};

