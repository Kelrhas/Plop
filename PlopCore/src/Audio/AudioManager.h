#pragma once

#include "Sound.h"

#include <AL/alc.h>
#include <Al/al.h>
#include <glm/vec3.hpp>
#include <unordered_map>

namespace Plop
{
	using SoundPtrMap = std::unordered_map<String, SoundPtr>;
	class AudioManager
	{
	public:
		AudioManager();
		~AudioManager();

		bool				Init();
		bool				Shutdown();
		void				DrawEditorPanel( bool* _bOpen );

		void				Reset();

		void				SetListenerPosition( glm::vec3 _vPos );



	private:
		ALCdevice*			m_pDevice;
		ALCcontext*			m_pContext;
		ALuint				m_uSourceID;
		int					m_iDeviceIndex;
		const ALCchar*		m_pAllDevices;

		float				m_fMasterVolume = 1.f;

	};

	extern AudioManager* g_pAudioManager;
}
