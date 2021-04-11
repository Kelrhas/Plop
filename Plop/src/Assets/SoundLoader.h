#pragma once

#include "Audio/Sound.h"

namespace Plop::AssetLoader
{
	SoundHandle GetSound( const StringPath& _sFilepath );

	void ClearSoundCache();

	SoundHandle PickSoundFromCache();
};
