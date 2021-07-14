#include "Plop_pch.h"
#include "ECS/Components/Component_AudioEmitter.h"


#include <al.h>
#include <alc.h>

#include "Assets/SoundLoader.h"
#include "Debug/Debug.h"


namespace Plop
{
	Component_AudioEmitter::Component_AudioEmitter( const Component_AudioEmitter& _other )
	{
		*this = _other;
	}
	Component_AudioEmitter::Component_AudioEmitter( Component_AudioEmitter&& _other )
	{
		*this = std::move(_other);
	}

	Component_AudioEmitter& Component_AudioEmitter::operator=( const Component_AudioEmitter& _other )
	{
		m_hSound = _other.m_hSound;

		return *this;
	}

	Component_AudioEmitter& Component_AudioEmitter::operator=( Component_AudioEmitter&& _other )
	{
		m_uSourceID = _other.m_uSourceID;
		m_hSound = _other.m_hSound;
		if (m_uSourceID)
			StopSound();

		_other.m_hSound = SoundHandle();
		_other.m_uSourceID = 0;
		return *this;
	}

	void Component_AudioEmitter::OnCreate()
	{
		alGenSources( 1, &m_uSourceID );
		Debug::Assert_AL();

		if (m_hSound)
			AttachSound( m_hSound );
	}

	void Component_AudioEmitter::OnDestroy()
	{
		if (m_uSourceID)
		{
			StopSound();
			alSourcei( m_uSourceID, AL_BUFFER, 0 );
			Debug::Assert_AL();
			alDeleteSources( 1, &m_uSourceID );
			Debug::Assert_AL();
			m_uSourceID = 0;
		}
	}

	void Component_AudioEmitter::AttachSound( SoundHandle _hSound )
	{
		m_hSound = _hSound;

		if(m_hSound)
			alSourcei( m_uSourceID, AL_BUFFER, m_hSound->GetBufferID() );
		else
			alSourcei( m_uSourceID, AL_BUFFER, 0 );
		Debug::Assert_AL();
	}

	void Component_AudioEmitter::PlaySound(bool _bResetIfPlaying /*= false*/)
	{
		//if( _bResetIfPlaying || !m_bPlaying )
		{
			alSourcePlay( m_uSourceID );
			Debug::Assert_AL();
		}

		m_bPlaying = true;
	}

	void Component_AudioEmitter::StopSound()
	{
		alSourceStop( m_uSourceID );
		Debug::Assert_AL();

		m_bPlaying = false;
	}

	void Component_AudioEmitter::PauseSound()
	{
		alSourcePause( m_uSourceID );
		Debug::Assert_AL();

		m_bPlaying = false;
	}
}


namespace MM
{
	template <>
	void ComponentEditorWidget<Plop::Component_AudioEmitter>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::Component_AudioEmitter>( e );
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
	json ComponentToJson<Plop::Component_AudioEmitter>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::Component_AudioEmitter>( e );
		json j;
		if (comp.m_hSound)
			j["Sound"] = comp.m_hSound->GetFilePathStr();
		return j;
	}

	template<>
	void ComponentFromJson<Plop::Component_AudioEmitter>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<Plop::Component_AudioEmitter>( e );
		if (_j.contains( "Sound" ))
		{
			String path;
			_j["Sound"].get_to( path );
			Plop::SoundHandle hSnd = Plop::AssetLoader::GetSound( path );
			comp.AttachSound( hSnd );
		}
	}
}