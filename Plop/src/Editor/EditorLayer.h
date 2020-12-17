#pragma once

#include <ApplicationLayer.h>
#include <Events/IEventListener.h>
#include <ECS/Entity.h>

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


				void		ShowMenuBar();
				void		ShowSceneGraph();
				void		PlayLevel();
				void		StopLevel();

		template<typename T>
				void		RegisterComponent( const char* _pName )
		{
			if(s_pENTTEditor)
				s_pENTTEditor->registerComponent<T>( _pName );
		}

		static	Entity		DuplicateEntity( const Entity& _entity );



		bool				m_bShowImGuiDemo = false;
		bool				m_bShowAllocations = false;
		bool				m_bLevelPlaying = false;
		Entity				m_SelectedEntity;
		EditMode m_eEditMode = EditMode::NONE;
		
		
		static ::MM::EntityEditor<entt::entity>* s_pENTTEditor;


	};
}
