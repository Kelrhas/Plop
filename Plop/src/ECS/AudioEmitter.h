#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>

#include "TimeStep.h"
#include "AUDIO/Sound.h"

#ifdef PlaySound
#undef PlaySound
#endif

namespace Plop
{
	struct AudioEmitterComponent
	{
		AudioEmitterComponent() = default;
		AudioEmitterComponent( const AudioEmitterComponent& _other );
		AudioEmitterComponent( AudioEmitterComponent&& _other );
		AudioEmitterComponent& operator=( const AudioEmitterComponent& _other );
		AudioEmitterComponent& operator=( AudioEmitterComponent&& _other );

		void OnCreate();
		void OnDestroy();

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

namespace MM
{
	template <>	void ComponentEditorWidget<Plop::AudioEmitterComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::AudioEmitterComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::AudioEmitterComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );

}