#pragma once

#include <entt/meta/factory.hpp>

#include "ApplicationLayer.h"
#include "Events/IEventListener.h"
#include "ECS/Entity.h"

namespace MM
{
	template<typename T>
	class EntityEditor;
}

namespace Plop
{
	class EditorLayer : public ApplicationLayer, IEventListener
	{
	private:
		friend class Application;
		EditorLayer();
		~EditorLayer();

	public:
		virtual void		OnRegistered() override;
		virtual void		OnUnregistered() override;
		virtual void		OnUpdate( TimeStep& _timeStep ) override;

		virtual bool		OnEvent( Event& _event ) override;

		virtual uint8_t		GetPriority() const { return 190; }


		static	json		GetJsonEntity( const Entity& _entity );
		static	void		SetJsonEntity( const Entity& _entity, const json& _j );


	private:
		enum class EditMode
		{
			NONE,
			RENAMING_ENTITY,
		};
		enum class LevelState
		{
			EDITING,
			STARTING, // for aync
			RUNNING,
			PAUSED,
			STOPPING // for aync
		};


				void		ShowMenuBar();
				void		ShowSceneGraph();
				void		ShowGizmos();

				void		NewLevel();
				void		OpenLevel();
				void		SaveLevel();
				void		SaveLevelAs();
				void		PlayLevel();
				void		PauseLevel();
				void		ResumeLevel();
				void		StopLevel();

		template<typename Comp, bool RegisterEditor>
				void		RegisterComponent( const char* _pName )
		{
			if constexpr (RegisterEditor == std::true_type::value)
			{
				if (s_pENTTEditor)
					s_pENTTEditor->registerComponent<Comp>( _pName );
			}

			auto factory = entt::meta<Comp>().type( entt::hashed_string( _pName ) );
			factory.func<&EditorLayer::CloneRegistryComponents<Comp>>( "clone"_hs );
		}
		template<typename Comp>
		static	void		CloneRegistryComponents( entt::registry& _src, entt::registry& _dst )
		{
			auto view = _src.view<Comp>();
			if constexpr (ENTT_IS_EMPTY( Comp )::value) {
				_dst.insert<Comp>( view.data(), view.data() + view.size() );
			}
			else {
				_dst.insert<Comp>( view.data(), view.data() + view.size(), view.raw(), view.raw() + view.size() );
			}
		}

		static	Entity		DuplicateEntity( const Entity& _entity );



		bool				m_bShowImGuiDemo = false;
		bool				m_bShowAllocations = false;
		LevelBasePtr		m_xCloneLevel = nullptr; // used when we Play the current level
		LevelBasePtr		m_xBackupLevel = nullptr;
		LevelState			m_eLevelState = LevelState::EDITING;
		Entity				m_SelectedEntity;
		EditMode			m_eEditMode = EditMode::NONE;
		StringPath			m_sCurrentLevel;
		
		
		static ::MM::EntityEditor<entt::entity>* s_pENTTEditor;


	};
}
