#pragma once

#include "Renderer/Texture.h"
#include "Types.h"

namespace Plop
{
	namespace AssetLoader
	{
		TextureHandle GetTexture( const StringPath& _sFilePath );

		TextureHandle PickTextureFromCache();
	};
}
