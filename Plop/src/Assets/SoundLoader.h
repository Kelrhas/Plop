#pragma once

#include "Audio/Sound.h"

namespace Plop
{
	namespace AssetLoader
	{
		SoundHandle GetSound( const StringPath& _sFilepath );

		SoundHandle PickSoundFromCache();
	};
}
