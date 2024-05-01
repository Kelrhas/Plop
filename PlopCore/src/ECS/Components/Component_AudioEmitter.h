#pragma once

#include "AUDIO/Sound.h"
#include "ECS/ComponentManager.h"
#include "TimeStep.h"

#ifdef PlaySound
#undef PlaySound
#endif

namespace Plop
{
	struct Component_AudioEmitter
	{
		Component_AudioEmitter() = default;
		Component_AudioEmitter( const Component_AudioEmitter& _other );
		Component_AudioEmitter( Component_AudioEmitter&& _other );
		Component_AudioEmitter& operator=( const Component_AudioEmitter& _other );
		Component_AudioEmitter& operator=( Component_AudioEmitter&& _other );

		void OnCreate(entt::registry &, entt::entity);
		void OnDestroy(entt::registry &, entt::entity);
		void EditorUI();
		json ToJson() const;
		void FromJson( const json& _j );

		void AttachSound( SoundHandle _hSound);
		void PlaySound( bool _bResetIfPlaying = false );
		void StopSound();
		void PauseSound();

	//private:
		bool m_bPlaying = false;
		ALuint m_uSourceID = 0;
		SoundHandle m_hSound;
	};
}
// TODO, component that emits sound with an openAL source
