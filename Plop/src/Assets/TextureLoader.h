#pragma once

#include <Renderer/Texture.h>

namespace Plop
{
	namespace AssetLoader
	{
		TextureHandle GetTexture( const String& _sFilename );

		TextureHandle PickTextureFromCache();
	};
}
