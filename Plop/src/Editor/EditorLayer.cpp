#include "Plop_pch.h"
#include "EditorLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo.h>
#include <misc/cpp/imgui_stdlib.h>
#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?
#include <imgui_entt_entity_editor.hpp>


#include "Application.h"
#include "Editor/Console.h"
#include "ECS/BaseComponents.h"
#include "ECS/LevelBase.h"
#include "Input/Input.h"
#include "Events/EventDispatcher.h"
#include "Events/EntityEvent.h"
#include "Utils/OSDialogs.h"


namespace Plop
{
	namespace Private
	{
		// Renaming
		static String sNewName;
	}

	::MM::EntityEditor<entt::entity>* EditorLayer::s_pENTTEditor = nullptr;

	EditorLayer::EditorLayer()
	{
		s_pENTTEditor = NEW ::MM::EntityEditor<entt::entity>;
	}

	EditorLayer::~EditorLayer()
	{
		delete s_pENTTEditor;
	}

	void EditorLayer::OnRegistered()
	{
		EventDispatcher::RegisterListener( this );
	}

	void EditorLayer::OnUnregistered()
	{
		EventDispatcher::UnregisterListener( this );
	}

	void EditorLayer::OnUpdate( TimeStep& _timeStep )
	{
		static ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar |
			ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground;

		ImGuiIO& io = ImGui::GetIO();
		if (!(io.ConfigFlags & ImGuiConfigFlags_DockingEnable))
			return;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos( viewport->GetWorkPos() );
		ImGui::SetNextWindowSize( viewport->GetWorkSize() );
		ImGui::SetNextWindowViewport( viewport->ID );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

		ImGui::Begin( "Editor", nullptr, windowFlags );
		{
			ImGui::PopStyleVar( 3 );

			static ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_PassthruCentralNode;
			static ImGuiID id = ImGui::GetID( "Editor" );
			ImGui::DockSpace( id, ImVec2( 0, 0 ), flags );

			ShowMenuBar();


			// we can draw editor windows here

			if (m_bShowImGuiDemo)
				ImGui::ShowDemoWindow( &m_bShowImGuiDemo );

			if (m_bShowAllocations)
				Debug::ShowAllocationsWindow( &m_bShowAllocations );

			// TODO set docking to bottom
			Console::Draw();


			if (m_bLevelPlaying == false)
			{
				ShowGizmos();
				ShowSceneGraph();

				if (m_SelectedEntity)
				{
					if (!LevelBase::s_xCurrentLevel.expired())
					{
						LevelBasePtr xLevel = LevelBase::s_xCurrentLevel.lock();
						if (!s_pENTTEditor->render( xLevel->m_ENTTRegistry, m_SelectedEntity.m_EntityId ))
							m_SelectedEntity = {};
					}
				}
			}
		}

		ImGui::End(); // Begin("Editor")
	}

	bool EditorLayer::OnEvent( Event& _event )
	{
		if (_event.GetEventType() == EventType::EntityDestroyedEvent)
		{
			EntityDestroyedEvent& entityEvent = (EntityDestroyedEvent&)_event;
			if (m_SelectedEntity == entityEvent.entity)
			{
				m_SelectedEntity.Reset();
			}
		}

		return false;
	}



	json EditorLayer::GetJsonEntity( const Entity& _entity )
	{
		json j;
		entt::entity entityID = _entity.m_EntityId;
		entt::registry& reg = _entity.m_xLevel.lock()->m_ENTTRegistry;

		for (auto& [component_type_id, ci] : s_pENTTEditor->component_infos)
		{
			if (s_pENTTEditor->entityHasComponent( reg, entityID, component_type_id ))
			{
				j[ci.name] = ci.tojson( reg, entityID );
			}
		}

		return j;
	}

	void EditorLayer::SetJsonEntity( const Entity& _entity, const json& _j )
	{
		entt::entity entityID = _entity.m_EntityId;
		entt::registry& reg = _entity.m_xLevel.lock()->m_ENTTRegistry;

		for (auto& [component_type_id, ci] : s_pENTTEditor->component_infos)
		{
			if (_j.contains( ci.name ))
			{
				ci.fromjson( reg, entityID, _j[ci.name] );
			}
		}
	}

	void EditorLayer::ShowMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu( "File" ))
			{
				if (ImGui::MenuItem( "New level", "Ctrl + N", nullptr ))
					NewLevel();

				if (ImGui::MenuItem( "Open ...", "Ctrl + O" ))
					OpenLevel();

				if (m_sCurrentLevel.empty())
				{
					if (ImGui::MenuItem( "Save ...", "Ctrl + S", nullptr, !LevelBase::GetCurrentLevel().expired() ))
						SaveLevelAs();
				}
				else
				{
					if (ImGui::MenuItem( "Save level", "Ctrl + S", nullptr, !LevelBase::GetCurrentLevel().expired() ))
						SaveLevel();

					if (ImGui::MenuItem( "Save as ...", "Ctrl + Shift + S", nullptr, !LevelBase::GetCurrentLevel().expired() ))
						SaveLevelAs();
				}

				ImGui::Separator();
				if (ImGui::MenuItem( "Exit" ))
					Application::Get()->Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu( "Help" ))
			{
				ImGui::MenuItem( "Show demo window", nullptr, &m_bShowImGuiDemo );

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu( "Debug" ))
			{
				ImGui::MenuItem( "Show allocations", nullptr, &m_bShowAllocations );

				ImGui::EndMenu();
			}

			if (m_bLevelPlaying)
			{
				if (ImGui::MenuItem( "Stop" ))
					StopLevel();
			}
			else
			{
				if (ImGui::MenuItem( "Play" ))
					PlayLevel();
			}

			ImGui::EndMenuBar();
		}

		// shortcuts for ImGui menus
		bool bControlDown = Input::IsKeyDown( KeyCode::KEY_Control );
		bool bShiftDown = Input::IsKeyDown( KeyCode::KEY_Shift );
		if (bControlDown)
		{
			if (Input::IsKeyPressed( KeyCode::KEY_N ))
				NewLevel();
			if (Input::IsKeyPressed( KeyCode::KEY_O ))
				OpenLevel();
			if (Input::IsKeyPressed( KeyCode::KEY_S ) && !LevelBase::GetCurrentLevel().expired())
			{
				if (bShiftDown || m_sCurrentLevel.empty())
					SaveLevelAs();
				else
					SaveLevel();
			}
		}

		if (m_SelectedEntity)
		{
			if (m_eEditMode == EditMode::NONE && Input::IsKeyPressed( KeyCode::KEY_F2 ))
			{
				Private::sNewName = m_SelectedEntity.GetComponent<NameComponent>().sName;
				m_eEditMode = EditMode::RENAMING_ENTITY;
			}
		}
	}

	void EditorLayer::ShowSceneGraph()
	{
		if (ImGui::Begin( "Scene graph" ))
		{
			if (!LevelBase::s_xCurrentLevel.expired())
			{
				static Entity entityToDestroy;
				static std::function<void( Entity& )> DrawEntity;
				DrawEntity = [this]( Entity& _Entity ) {

					ImGui::PushID( entt::to_integral( _Entity.m_EntityId ) );

					String& sName = _Entity.GetComponent<NameComponent>().sName;
					if (m_eEditMode == EditMode::RENAMING_ENTITY && _Entity == m_SelectedEntity)
					{
						ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, { 0, 0 } );
						ImGui::SetItemDefaultFocus();
						if (ImGui::InputText( "##RenameEntity", &Private::sNewName, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll ))
						{
							sName = Private::sNewName;
							m_eEditMode = EditMode::NONE;
						}

						if (ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_Escape ) ) || // ask imgui as it captures the input from Input::
							!ImGui::IsWindowFocused( ImGuiFocusedFlags_RootAndChildWindows ) ||
							ImGui::IsItemDeactivated())
						{
							m_eEditMode = EditMode::NONE;
						}

						ImGui::PopStyleVar();
					}
					else
					{
						if (ImGui::Selectable( sName.c_str(), m_SelectedEntity == _Entity ))
						{
							m_SelectedEntity = _Entity;
							m_eEditMode = EditMode::NONE;
						}
					}

					if (ImGui::BeginPopupContextItem( "EntityContextMenu" ))
					{
						if (ImGui::Selectable( "New entity" ))
						{
							LevelBasePtr xLevel = LevelBase::s_xCurrentLevel.lock();
							Entity e = xLevel->CreateEntity();
							e.SetParent( _Entity );
						}
						if (ImGui::Selectable( "Duplicate entity" ))
						{
							DuplicateEntity( _Entity );
						}
						if (ImGui::Selectable( "Delete entity" ))
							entityToDestroy = _Entity;

						ImGui::EndPopup();
					}
					else
					{
						if (ImGui::BeginDragDropSource( ImGuiDragDropFlags_None ))
						{
							ImGui::SetDragDropPayload( "ReparentEntity", &_Entity.m_EntityId, sizeof( _Entity.m_EntityId ) );

							ImGui::Text( sName.c_str() );
							ImGui::EndDragDropSource();
						}
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload( "ReparentEntity" ))
							{
								ASSERT( pPayload->DataSize == sizeof( entt::entity ), "Wrong Drag&Drop payload" );
								Entity child( *(entt::entity*)pPayload->Data, _Entity.m_xLevel );

								if (child.GetParent() != _Entity)
									child.SetParent( _Entity );
								else
									child.SetParent( Entity{ entt::null, _Entity.m_xLevel } );
							}
							ImGui::EndDragDropTarget();
						}
					}

					ImGui::PopID();

					ImGui::Indent();
					for (Entity& child : _Entity.GetChildren())
					{
						DrawEntity( child );
					}
					ImGui::Unindent();
				};

				ImGui::BeginChild( "GraphNodeList", ImVec2( 0, -ImGui::GetFrameHeightWithSpacing() ) );

				LevelBasePtr xLevel = LevelBase::s_xCurrentLevel.lock();

				auto& view = xLevel->m_ENTTRegistry.view<NameComponent>();
				for (auto& e : view)
				{
					Entity entity{ e, LevelBase::s_xCurrentLevel };
					Entity parent = entity.GetParent();

					// only draw those without parent, and each one will draw their children
					if (!parent)
					{
						DrawEntity( entity );
					}
				}

				if (entityToDestroy)
				{
					LevelBase::s_xCurrentLevel.lock()->DestroyEntity( entityToDestroy );
					entityToDestroy.Reset();
				}

				ImGui::EndChild();
				if (ImGui::Button( "New entity" ))
				{
					xLevel->CreateEntity();
				}
			}
		}
		ImGui::End();

	}

		ImVec2 vSize = ImGui::GetMainViewport()->Size;
		ImGuizmo::SetRect( vPosition.x, vPosition.y, vSize.x, vSize.y );
#else
		ImVec2 vSize = ImGui::GetIO().DisplaySize;
		ImGuizmo::SetRect( 0, 0, vSize.x, vSize.y );
#endif

		// get the current camera, TODO editor camera
		CameraPtr xCurrentCamera = nullptr;
		glm::mat4 mViewMatrix = glm::identity<glm::mat4>();
		glm::mat4 mProjMatrix = glm::identity<glm::mat4>();
		LevelBasePtr xLevel = LevelBase::s_xCurrentLevel.lock();
		if (xLevel)
		{
			auto& view = xLevel->GetEntityRegistry().view<CameraComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto& [camera, transform] = view.get<CameraComponent, TransformComponent>( entity );
				xCurrentCamera = camera.xCamera;
				mViewMatrix = glm::inverse( transform.GetMatrix() );
				mProjMatrix = camera.xCamera->GetProjectionMatrix();
			}

			if (xCurrentCamera)
			{
				if (m_SelectedEntity)
				{
					ImGuizmo::SetOrthographic( xCurrentCamera->IsOrthographic() );
					glm::mat4 mTransform = m_SelectedEntity.GetComponent<TransformComponent>().GetMatrix();
					if (ImGuizmo::Manipulate( glm::value_ptr( mViewMatrix ), glm::value_ptr( mProjMatrix ),
						ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr( mTransform ) ))
					{
						m_SelectedEntity.GetComponent<TransformComponent>().SetMatrix( mTransform );
					}
				}

				if (false)
				{
					// TODO Position the view square correctly with the panels and fixed offsets from the top and right
					// TODO actually make it work by decomposing the viewmatrix back to camera position/rotation
					static float fXMul = 0.703f;
					static float fYMul = 0.032f;

#ifdef IMGUI_HAS_VIEWPORT
					ImVec2 vPosition = ImGui::GetMainViewport()->Size;
					vPosition.x *= fXMul;
					vPosition.y *= fYMul;
					vPosition.x += ImGui::GetMainViewport()->Pos.x;
					vPosition.y += ImGui::GetMainViewport()->Pos.y;
#else
					ImVec2 vPosition = io.DisplaySize;
					vPosition.x *= fXMul;
					vPosition.y *= fYMul;
#endif
					ImGuizmo::ViewManipulate( glm::value_ptr( mViewMatrix ), 1.f, vPosition, ImVec2( 128, 128 ), 0x10101010 );
				}
			}
		}


		if (false)
		{
			static const glm::mat4 mIdentity = glm::identity<glm::mat4>();
			ImGuizmo::DrawGrid( glm::value_ptr( mViewMatrix ), glm::value_ptr( mProjMatrix ), glm::value_ptr( mIdentity ), 10.f );
		}
	}

	void EditorLayer::NewLevel()
	{
		auto xLevel = Application::Get()->CreateNewLevel();
		m_SelectedEntity.Reset();
		xLevel->MakeCurrent();
		m_sCurrentLevel.clear();
	}

	void EditorLayer::OpenLevel()
	{
		StringPath sLevelPath;
		if (Dialog::OpenFile( sLevelPath, Dialog::LEVEL_FILTER ))
		{
			auto xLevel = Application::Get()->CreateNewLevel();
			m_SelectedEntity.Reset();
			xLevel->MakeCurrent();
			xLevel->Load( sLevelPath );
			m_sCurrentLevel = sLevelPath;
		}
	}

	void EditorLayer::SaveLevel()
	{
		if(!m_sCurrentLevel.empty())
			LevelBase::GetCurrentLevel().lock()->Save( m_sCurrentLevel );
	}

	void EditorLayer::SaveLevelAs()
	{
		StringPath sLevelPath;
		if (Dialog::SaveFile( sLevelPath, Dialog::LEVEL_FILTER ))
		{
			// check for extension
			if (sLevelPath.extension() != ".lvl")
				sLevelPath += ".lvl";


			LevelBase::GetCurrentLevel().lock()->Save( sLevelPath );
			m_sCurrentLevel = sLevelPath;
		}
	}

	void EditorLayer::PlayLevel()
	{
		LevelBasePtr xLevel = LevelBase::s_xCurrentLevel.lock();
		xLevel->StartFromEditor();
		m_bLevelPlaying = true;
	}

	void EditorLayer::StopLevel()
	{
		LevelBasePtr xLevel = LevelBase::s_xCurrentLevel.lock();
		xLevel->StopToEditor();
		m_bLevelPlaying = false;
	}

	Entity EditorLayer::DuplicateEntity( const Entity& _entity )
	{
		LevelBasePtr xLevel = LevelBase::s_xCurrentLevel.lock();
		const String& sName = _entity.GetComponent<NameComponent>().sName;
		Entity dupEntity = xLevel->CreateEntity( sName );
		dupEntity.SetParent( _entity.GetParent() );

		entt::registry& reg = xLevel->m_ENTTRegistry;

		for (auto& [component_type_id, ci] : s_pENTTEditor->component_infos)
		{
			ci.duplicate( reg, _entity, dupEntity );
		}

		for (const Entity& _child : _entity.GetChildren())
		{
			Entity dupChild = DuplicateEntity( _child );
			dupChild.SetParent( dupEntity );
		}

		return dupEntity;
	}
}
