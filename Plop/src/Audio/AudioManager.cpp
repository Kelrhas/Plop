#include "Plop_pch.h"
#include "AudioManager.h"

#include "Assets/SoundLoader.h"
#include "Debug/Log.h"
#include "ECS/LevelBase.h"
#include "ECS/AudioEmitter.h"

#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?

namespace Plop
{
	AudioManager* g_pAudioManager = nullptr;

	AudioManager::AudioManager()
		: m_pDevice( nullptr )
		, m_pContext( nullptr )
		, m_uSourceID( 0 )
		, m_iDeviceIndex( -1 )
		, m_pAllDevices( nullptr )
	{}

	AudioManager::~AudioManager()
	{
	}

	static void list_audio_devices( const ALCchar *devices )
	{
		const ALCchar *device = devices, *next = devices + 1;
		size_t len = 0;

		Log::Info( "Devices list:\n" );
		Log::Info( "----------\n" );
		while( device && *device != '\0' && next && *next != '\0' )
		{
			Log::Info( "%s\n", device );
			len = strlen( device );
			device += (len + 1);
			next += (len + 2);
		}
		Log::Info( "----------\n" );
	}

	bool AudioManager::Init()
	{
		ASSERTM( m_pDevice == nullptr, "Audio device already exists" );
		if (m_pDevice != nullptr)
			return false;

		ASSERT( alcIsExtensionPresent( NULL, "ALC_enumeration_EXT" ) == AL_TRUE );
		
		if( alcIsExtensionPresent( NULL, "ALC_enumerate_all_EXT" ) )
			m_pAllDevices = alcGetString( NULL, ALC_ALL_DEVICES_SPECIFIER );
		else
			m_pAllDevices = alcGetString( NULL, ALC_DEVICE_SPECIFIER );
		Debug::Assert_AL();

		if( m_iDeviceIndex >= 0 )
		{
			const ALCchar* sDevice = m_pAllDevices;
			for( int i = 0; i < m_iDeviceIndex; ++i )
				sDevice += strlen( sDevice ) + 1;
			m_pDevice = alcOpenDevice( sDevice );
		}
		else
		{
			m_iDeviceIndex = 0;
			m_pDevice = alcOpenDevice( NULL );
		}
		Debug::Assert_AL();
		if (m_pDevice == nullptr)
			return false;

		m_pContext = alcCreateContext( m_pDevice, nullptr );
		if( m_pContext == nullptr )
			return false;

		if (alcMakeContextCurrent( m_pContext ) == false)
			return false;

		Debug::Assert_AL();

		alGenSources( 1, &m_uSourceID );
		Debug::Assert_AL();

		alGetListenerf( AL_GAIN, &m_fMasterVolume );
		Debug::Assert_AL();

		return true;
	}

	bool AudioManager::Shutdown()
	{
		auto xLevel = LevelBase::GetCurrentLevel().lock();
		if (xLevel)
		{
			auto view = xLevel->GetEntityRegistry().view<AudioEmitterComponent>();
			for (auto [entity, audioComp] : view.proxy())
			{
				audioComp.AttachSound( SoundHandle{} );
			}
		}

		alSourceStop( m_uSourceID );
		Debug::Assert_AL();
		alSourcei( m_uSourceID, AL_BUFFER, 0 );
		Debug::Assert_AL();
		alDeleteSources( 1, &m_uSourceID );
		m_uSourceID = 0;
		Debug::Assert_AL();

		alcMakeContextCurrent( nullptr );
		Debug::Assert_AL();
		alcDestroyContext( m_pContext );
		Debug::Assert_AL();
		m_pContext = nullptr;
		alcCloseDevice( m_pDevice );
		Debug::Assert_AL();
		m_pDevice = nullptr;

		return true;
	}

	void AudioManager::DrawEditorPanel( bool* _bOpen )
	{
		if (ImGui::Begin( "Audio manager", _bOpen ))
		{
			if (ImGui::Combo( "Devices", &m_iDeviceIndex, m_pAllDevices ))
			{
				Reset();
			}

			if (ImGui::SliderFloat( "Master volume", &m_fMasterVolume, 0.f, 1.f ))
				alListenerf( AL_GAIN, m_fMasterVolume );


			if (ImGui::Button( "Test sound" ))
			{
				SoundHandle hSnd = AssetLoader::GetSound( "D:\\Prog\\Plop\\data\\audio\\test.wav" );
				alSourcei( m_uSourceID, AL_BUFFER, hSnd->GetBufferID() );
				Debug::Assert_AL();
				alSourcePlay( m_uSourceID );
				Debug::Assert_AL();
			}

			if (ImGui::Button( "Test sound on all emitters" ))
			{
				auto& reg = LevelBase::GetCurrentLevel().lock()->GetEntityRegistry();
				auto& view = reg.view<AudioEmitterComponent>();
				view.each( [&]( AudioEmitterComponent& emitter) {

					SoundHandle hSnd = AssetLoader::GetSound( "D:\\Prog\\Plop\\data\\audio\\test.wav" );
					emitter.AttachSound( hSnd );
					emitter.PlaySound( true );
				});
			}
		}
		ImGui::End();
	}

	void AudioManager::Reset()
	{
		VERIFY( Shutdown() );

		VERIFY( Init() );
	}

	void AudioManager::SetListenerPosition( glm::vec3 _vPos )
	{
		static_assert(sizeof( _vPos.x ) == sizeof( ALfloat ), "Not the same size");
		alListenerfv( AL_POSITION, &_vPos.x );
		Debug::Assert_AL();
	}
}