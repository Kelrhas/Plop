#pragma once

#include <entt/meta/factory.hpp>

#include "ApplicationLayer.h"
#include "Events/IEventListener.h"
#include "ECS/Entity.h"
#include "Editor/EditorCamera.h"

namespace MM
{
	template<typename T>
	class EntityEditor;
}

namespace Plop
{
	class EditorGizmo;

	class EditorLayer : public ApplicationLayer, IEventListener
	{
	private:
		friend class Application;
		friend class EditorGizmo;
		EditorLayer();
		~EditorLayer();

	public:
		virtual void			OnRegistered() override;
		virtual void			OnUnregistered() override;
		virtual void			OnUpdate( TimeStep& _timeStep ) override;
		virtual void			OnImGuiRender( TimeStep& _timeStep ) override;

		virtual bool			OnEvent( Event& _event ) override;

		virtual uint8_t			GetPriority() const { return 190; }

				EditorCameraPtr GetEditorCamera() const { return m_xEditorCamera; }
				glm::vec2		GetViewportPosFromWindowPos( const glm::vec2& _vScreenPos, bool _bClamp = false ) const;

				bool			IsEditing() const { return m_eLevelState == LevelState::EDITING; }

		static	json			GetJsonEntity( const Entity& _entity );
		static	void			SetJsonEntity( const Entity& _entity, const json& _j );

				void			PreventPicking() { m_bAllowPicking = false; }
				void			RestorePicking() { m_bAllowPicking = true; }


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

		struct EntityEditorInfo
		{
			bool bHierarchyOpen = false;
		};

				void		ShowMenuBar();
				void		ShowToolBar();
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


				bool		IsSelectedEntityForLevel(LevelBaseWeakPtr _xLevel) const;

		template<typename Comp, bool RegisterEditor>
				void		RegisterComponent( const char* _pName )
		{

#ifndef USE_COMPONENT_MGR
			if constexpr (RegisterEditor == std::true_type::value)
			{
				if (s_pENTTEditor)
					s_pENTTEditor->registerComponent<Comp>( _pName );
			}
#endif

			auto factory = entt::meta<Comp>().type( entt::hashed_string( _pName ) );
			factory.func<&EditorLayer::CloneRegistryComponents<Comp>>( "cloneAllComponents"_hs );
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
		static	void		CopyEntity(const Entity& _entity );
		static	bool		CanPasteEntity(const Entity& _entityParent);
		static	Entity		PasteEntity(Entity& _entityParent);
		static	void		ToggleEntity(Entity& _entity);



		bool				m_bShowImGuiDemo = false;
		bool				m_bShowAllocations = false;
		bool				m_bShowAudioDebug = false;
		bool				m_bShowCameraSettings = false;


		EditorCameraPtr		m_xEditorCamera;
		LevelBasePtr		m_xEditingLevel = nullptr;
		LevelState			m_eLevelState = LevelState::EDITING;
		Entity				m_SelectedEntity;
		bool				m_bAllowPicking = true;
		EditMode			m_eEditMode = EditMode::NONE;
		StringPath			m_sCurrentLevel;

		// viewport
		glm::vec2			m_vViewportSize = VEC2_0;
		glm::vec2			m_vViewportPosMinWindowSpace = VEC2_0;	// position on the app window
		glm::vec2			m_vViewportPosMaxWindowSpace = VEC2_0;	// position on the app window
		glm::vec2			m_vViewportPosMinScreenSpace = VEC2_0;	// position on the physical screen
		glm::vec2			m_vViewportPosMaxScreenSpace = VEC2_0;	// position on the physical screen
		
		
		static ::MM::EntityEditor<entt::entity>* s_pENTTEditor;

		std::unordered_map<entt::entity, EntityEditorInfo> m_mapEntityEditorInfo;

	};


	class EditorGizmo
	{
	public:
		static void Point( const glm::vec2& _vPoint, glm::vec3 _vColor = COLOR_WHITE );
		static void Circle( const glm::vec3& _vPos, float _fRadius, glm::vec3 _vColor = COLOR_WHITE);
		static void FilledCircle( const glm::vec3& _vPos, float _fRadius, glm::vec3 _vColor = COLOR_WHITE);

		// Lines & Curves
		static void Line( const glm::vec3& _v1, const glm::vec3& _v2, glm::vec3 _vColor = COLOR_WHITE);
		static void AABB( const glm::vec3& _vMin, const glm::vec3& _vMax, glm::vec3 _vColor = COLOR_WHITE);
		static void Cube( const glm::vec3& _vHalfSize, const glm::mat4 &_mTransform, glm::vec3 _vColor = COLOR_WHITE);
		static void Bezier( const glm::vec2& _v1, const glm::vec2& _v2, const glm::vec2& _v3, const glm::vec2& _v4, glm::vec3 _vColor = VEC3_1 );
		static void CatmullRom( const glm::vec3& _v1, const glm::vec3& _v2, const glm::vec3& _v3, const glm::vec3& _v4, glm::vec3 _vColor = VEC3_1 );


		static void SetCamera(CameraWeakPtr _xCamera);
		static void SetViewportPosAndSize( const glm::vec2& _vPos, const glm::vec2& _vSize );

	private:
		static glm::vec2 GetSSPosition( const glm::vec3& _vPos );
		static float GetSSDistance( const glm::vec3& _vPos, float _fDist );


		static CameraWeakPtr	s_xCamera;
		static glm::mat4		s_mViewProj; // cache from camera
		static glm::vec2		s_vViewportPos;
		static glm::vec2		s_vViewportSize;
	};
}
