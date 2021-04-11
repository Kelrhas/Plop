#pragma once

#include <al.h>
#include <alc.h>
#include <unordered_map>

#include "Sound.h"

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
