#pragma once

#ifdef USE_COMPONENT_MGR
#include <ECS/ComponentManager.h>
#else
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <imgui_entt_entity_editor.hpp>
#endif

#include "TimeStep.h"
#include "AUDIO/Sound.h"

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

		void OnCreate();
		void OnDestroy();
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

#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>	void ComponentEditorWidget<Plop::Component_AudioEmitter>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::Component_AudioEmitter>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::Component_AudioEmitter>( entt::registry& reg, entt::registry::entity_type e, const json& _j );

}
#endif