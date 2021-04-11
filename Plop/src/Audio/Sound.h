#pragma once

#include <al.h>
#include <alc.h>
#include <entt/resource/handle.hpp>

#include "Assets/Asset.h"

void Assert_AL();

namespace Plop
{
	class Sound;

	using SoundWeakPtr = std::weak_ptr<Sound>;
	using SoundPtr = std::shared_ptr<Sound>;
	using SoundHandle = entt::resource_handle<Sound>;

	class Sound : public Asset
	{
		friend class AudioManager;
		friend struct SoundLoader;

	public:
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