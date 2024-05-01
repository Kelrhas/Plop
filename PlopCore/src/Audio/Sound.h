#pragma once

#include "Assets/Asset.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <entt/resource/resource.hpp>

namespace Plop
{
	class Sound;

	using SoundWeakPtr = std::weak_ptr<Sound>;
	using SoundPtr = std::shared_ptr<Sound>;
	using SoundHandle = entt::resource<Sound>;

	namespace AssetLoader
	{
		struct SoundLoader;
	}

	class Sound : public Asset
	{
		friend class AudioManager;
		friend struct AssetLoader::SoundLoader;

	public:
		Sound() = default;
		Sound( const Sound& ) = delete;
		Sound( Sound&& ) = delete;
		virtual ~Sound();


		ALuint	GetBufferID() const { return m_uBufferID; }

	private:
		void	Reset();



		ALint		m_iChannels = 0;
		ALenum		m_eFormat = AL_NONE;
		ALuint		m_uBufferID = 0;
		ALubyte*	m_pSamples = nullptr;
		uint32_t	m_uDataSize = 0;
		uint32_t	m_uFrequency = 0;
	};
}