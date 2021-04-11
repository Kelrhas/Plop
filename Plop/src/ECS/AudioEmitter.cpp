#include "Plop_pch.h"
#include "AudioEmitter.h"


#include <al.h>
#include <alc.h>

#include "Assets/SoundLoader.h"
#include "Debug/Debug.h"


namespace Plop
{
	AudioEmitterComponent::AudioEmitterComponent( const AudioEmitterComponent& _other )
	{
		*this = _other;
	}
	AudioEmitterComponent::AudioEmitterComponent( AudioEmitterComponent&& _other )
	{
		*this = std::move(_other);
	}

	AudioEmitterComponent& AudioEmitterComponent::operator=( const AudioEmitterComponent& _other )
	{
		m_hSound = _other.m_hSound;

		return *this;
	}

	AudioEmitterComponent& AudioEmitterComponent::operator=( AudioEmitterComponent&& _other )
	{
		m_uSourceID = _other.m_uSourceID;
		m_hSound = _other.m_hSound;
		if (m_uSourceID)
			StopSound();

		_other.m_hSound = SoundHandle();
		_other.m_uSourceID = 0;
		return *this;
	}

	void AudioEmitterComponent::OnCreate()
	{
		alGenSources( 1, &m_uSourceID );
		Debug::Assert_AL();

		if (m_hSound)
			AttachSound( m_hSound );
	}

	void AudioEmitterComponent::OnDestroy()
	{
		if (m_uSourceID)
		{
			alSourcei( m_uSourceID, AL_BUFFER, 0 );
			Debug::Assert_AL();
			alDeleteSources( 1, &m_uSourceID );
			Debug::Assert_AL();
			m_uSourceID = 0;
		}
	}

	void AudioEmitterComponent::AttachSound( SoundHandle _hSound )
	{
		m_hSound = _hSound;

		if(m_hSound)
			alSourcei( m_uSourceID, AL_BUFFER, m_hSound->GetBufferID() );
		else
			alSourcei( m_uSourceID, AL_BUFFER, 0 );
		Debug::Assert_AL();
	}

	void AudioEmitterComponent::PlaySound(bool _bResetIfPlaying /*= false*/)
	{
		//if( _bResetIfPlaying || !m_bPlaying )
		{
			alSourcePlay( m_uSourceID );
			Debug::Assert_AL();
		}

		m_bPlaying = true;
	}

	void AudioEmitterComponent::StopSound()
	{
		alSourceStop( m_uSourceID );
		Debug::Assert_AL();

		m_bPlaying = false;
	}

	void AudioEmitterComponent::PauseSound()
	{
		alSourcePause( m_uSourceID );
		Debug::Assert_AL();

		m_bPlaying = false;
	}
}


namespace MM
{
	template <>
	void ComponentEditorWidget<Plop::AudioEmitterComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::AudioEmitterComponent>( e );
		ImGui::Text( "Source id : %d", comp.m_uSourceID );
		if (comp.m_hSound)
		{
			ImGui::Text( comp.m_hSound->GetFilePathStr().c_str() );
			if (ImGui::Button( "Play sound" ))
				comp.PlaySound( true );

			ImGui::SameLine();
		}

		if (ImGui::Button( "Load sound" ))
			ImGui::OpenPopup( "###PickSoundFromCache" );

		Plop::SoundHandle hNewSound = Plop::AssetLoader::PickSoundFromCache();
		if (hNewSound)
		{
			comp.AttachSound( hNewSound );
		}
	}

	template <>
	json ComponentToJson<Plop::AudioEmitterComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::AudioEmitterComponent>( e );
		json j;
		if (comp.m_hSound)
			j["Sound"] = comp.m_hSound->GetFilePathStr();
		return j;
	}

	template<>
	void ComponentFromJson<Plop::AudioEmitterComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Plop::AudioEmitterComponent>( e );
		if (_j.contains( "Sound" ))
		{
			String path;
			_j["Sound"].get_to( path );
			Plop::SoundHandle hSnd = Plop::AssetLoader::GetSound( path );
			comp.AttachSound( hSnd );
		}
	}
}