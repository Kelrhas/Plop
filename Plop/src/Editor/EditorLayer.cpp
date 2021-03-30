#include "Plop_pch.h"
#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_custom.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <ImGuizmo.h>
#include <misc/cpp/imgui_stdlib.h>
#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?
#include <imgui_entt_entity_editor.hpp>
#include <entt/meta/meta.hpp>


#include "Application.h"
#include "Editor/Console.h"
#include "ECS/BaseComponents.h"
#include "ECS/TransformComponent.h"
#include "ECS/LevelBase.h"
#include "Input/Input.h"
#include "Renderer/ParticleSystem.h"
#include "Events/EventDispatcher.h"
#include "Events/EntityEvent.h"
#include "Utils/OSDialogs.h"
#include "Assets/TextureLoader.h"
#include "Renderer/Texture.h"


namespace Plop
{
	namespace Private
	{
		static String sNewName; // For renaming
		static TextureHandle hIconSpriteSheet;
		static const ImVec2 vEditorIconSize( 48, 48 );
		static std::unordered_map<String, std::pair<ImVec2,ImVec2>> mapIconsUV = {
			{"Play",		{{0.f, 0.f},	{1.f / 3, 0.5f}}},
			{"Pause",		{{1.f/3, 0.f},	{2.f / 3, 0.5f}}},
			{"Stop",		{{2.f/3, 0.f},	{1.f, 0.5f}}},
			{"Translate",	{{0., 0.5f},	{1.f/3, 1.f}}},
			{"Rotate",		{{1.f/3, 0.5f},	{2.f/3, 1.f}}},
			{"Scale",		{{2.f/3, 0.5f},	{1.f, 1.f}}},
		};
		static ImGuizmo::OPERATION eGuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		static ImGuizmo::MODE eGuizmoSpace = ImGuizmo::MODE::LOCAL;
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

		Private::hIconSpriteSheet = AssetLoader::GetTexture( "Plop\\assets\\icons\\editor.png" );
	}

	void EditorLayer::OnUnregistered()
	{
		EventDispatcher::UnregisterListener( this );
	}

	void EditorLayer::OnUpdate( TimeStep& _timeStep )
	{
		static ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoTitleBar | /*ImGuiWindowFlags_MenuBar |*/
			ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_NoBackground;

		ImGuiIO& io = ImGui::GetIO();
		if (!(io.ConfigFlags & ImGuiConfigFlags_DockingEnable))
			return;


		ShowMenuBar();
		ShowToolBar();

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


			// we can draw editor windows here

			if (m_bShowImGuiDemo)
				ImGui::ShowDemoWindow( &m_bShowImGuiDemo );

			if (m_bShowAllocations)
				Debug::ShowAllocationsWindow( &m_bShowAllocations );

			// TODO set docking to bottom
			Console::Draw();


			if (m_eLevelState == LevelState::EDITING ||
				m_eLevelState == LevelState::PAUSED)
			{
				ShowGizmos();
				ShowSceneGraph();

				if (m_SelectedEntity && !LevelBase::s_xCurrentLevel.expired())
				{
					LevelBasePtr xLevel = LevelBase::s_xCurrentLevel.lock();
					if (m_SelectedEntity.m_xLevel.lock() == xLevel)
					{
						if (!s_pENTTEditor->render( xLevel->m_ENTTRegistry, m_SelectedEntity.m_EntityId ))
							m_SelectedEntity = {};
					}
				}
			}
		}

		ImGui::End(); // Begin("Editor")

		if (m_SelectedEntity && m_SelectedEntity.m_xLevel.lock() == LevelBase::GetCurrentLevel().lock())
		{
			if (m_SelectedEntity.HasComponent<ParticleSystemComponent>())
			{
				m_SelectedEntity.GetComponent<ParticleSystemComponent>().Update( _timeStep );
			}
		}
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


	bool IconButton(const char* _pButton, bool bActive = false)
	{
		if(!bActive)
			ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
		ImGui::PushID( _pButton );
		bool bClicked = ImGui::ImageButton( (ImTextureID)Private::hIconSpriteSheet->GetNativeHandle(), Private::vEditorIconSize, Private::mapIconsUV[_pButton].first, Private::mapIconsUV[_pButton].second, 0 );
		ImGui::PopID();
		if (!bActive)
			ImGui::PopStyleColor();

		return bClicked;
	}


	void EditorLayer::ShowToolBar()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		float fBarHeight = Private::vEditorIconSize.y + 2 * ImGui::GetStyle().WindowPadding.y;
		ImGui::SetNextWindowPos( viewport->GetWorkPos() );
		ImGui::SetNextWindowSize( ImVec2( viewport->GetWorkSize().x, fBarHeight ) );
		ImGui::SetNextWindowViewport( viewport->ID );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.f );
		if (ImGui::Begin( "##Manipulation", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize ))
		{
			ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

			if (IconButton( "Translate", Private::eGuizmoOperation == ImGuizmo::OPERATION::TRANSLATE ))
				Private::eGuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

			ImGui::SameLine();
			if (IconButton( "Rotate", Private::eGuizmoOperation == ImGuizmo::OPERATION::ROTATE ))
				Private::eGuizmoOperation = ImGuizmo::OPERATION::ROTATE;

			ImGui::SameLine();
			if (IconButton( "Scale", Private::eGuizmoOperation == ImGuizmo::OPERATION::SCALE ))
				Private::eGuizmoOperation = ImGuizmo::OPERATION::SCALE;





			ImGui::SameLine(0, 60);
			ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, Private::vEditorIconSize.y / 4.f );
			{
				bool bWasLocal = Private::eGuizmoSpace == ImGuizmo::MODE::LOCAL;
				if (!bWasLocal)
					ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );

				if (ImGui::ButtonRounded( "Local", ImVec2( 0, Private::vEditorIconSize.y ), ImDrawCornerFlags_Left ))
					Private::eGuizmoSpace = ImGuizmo::MODE::LOCAL;

				if (!bWasLocal)
					ImGui::PopStyleColor();
				else
					ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );

				ImGui::SameLine();
				if (ImGui::ButtonRounded( "World", ImVec2( 0, Private::vEditorIconSize.y ), ImDrawCornerFlags_Right ))
					Private::eGuizmoSpace = ImGuizmo::MODE::WORLD;

				if(bWasLocal)
					ImGui::PopStyleColor(); // ImGuiCol_Button
			}
			ImGui::PopStyleVar(); // ImGuiStyleVar_FrameRounding




			ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing

			ImGui::SameLine( 0, 60 );
			if (m_eLevelState == LevelState::RUNNING)
			{
				if (IconButton( "Pause" ))
					PauseLevel();

				ImGui::SameLine();
				if (IconButton( "Stop" ))
					StopLevel();
			}
			else if (m_eLevelState == LevelState::PAUSED)
			{
				if (IconButton( "Play" ))
					ResumeLevel();

				ImGui::SameLine();
				if (IconButton( "Stop" ))
					StopLevel();
			}
			else if (m_eLevelState == LevelState::EDITING)
			{
				if (IconButton( "Play" ))
					PlayLevel();
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();

		viewport->WorkOffsetMin.y += fBarHeight;
	}

	void EditorLayer::ShowMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
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

			ImGui::EndMainMenuBar();
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
								ASSERTM( pPayload->DataSize == sizeof( entt::entity ), "Wrong Drag&Drop payload" );
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
					entityToDestroy.Destroy();

				ImGui::EndChild();
				if (ImGui::Button( "New entity" ))
				{
					xLevel->CreateEntity();
				}
			}
		}
		ImGui::End();

	}

	void EditorLayer::ShowGizmos()
	{
		// update the viewport size
#ifdef IMGUI_HAS_VIEWPORT
		ImVec2 vPosition = ImGui::GetMainViewport()->Pos;
		ImVec2 vSize = ImGui::GetMainViewport()->Size;
		ImGuizmo::SetRect( vPosition.x, vPosition.y, vSize.x, vSize.y );
		EditorGizmo::SetViewportPosAndSize( vPosition, vSize );
#else
		ImVec2 vSize = ImGui::GetIO().DisplaySize;
		ImGuizmo::SetRect( 0, 0, vSize.x, vSize.y );
#endif
		ImGuizmo::SetDrawlist( ImGui::GetBackgroundDrawList() );

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
				mViewMatrix = glm::inverse( transform.GetWorldMatrix() );
				mProjMatrix = camera.xCamera->GetProjectionMatrix();
			}

			if (xCurrentCamera)
			{
				if (m_SelectedEntity && m_SelectedEntity.m_xLevel.lock() == LevelBase::GetCurrentLevel().lock())
				{
					ImGuizmo::SetOrthographic( xCurrentCamera->IsOrthographic() );
					glm::mat4 mTransform = m_SelectedEntity.GetComponent<TransformComponent>().GetWorldMatrix();

					if (ImGuizmo::Manipulate( glm::value_ptr( mViewMatrix ), glm::value_ptr( mProjMatrix ),
						Private::eGuizmoOperation, Private::eGuizmoSpace, glm::value_ptr( mTransform ) ))
					{
						m_SelectedEntity.GetComponent<TransformComponent>().SetWorldMatrix( mTransform );
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


				EditorGizmo::SetViewProjMatrix( mViewMatrix, mProjMatrix );
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
		m_eLevelState = LevelState::STARTING;
	}

	void EditorLayer::PauseLevel()
	{
		m_eLevelState = LevelState::PAUSED;
	}

	void EditorLayer::ResumeLevel()
	{
		m_eLevelState = LevelState::RUNNING;
	}

	void EditorLayer::StopLevel()
	{
		m_eLevelState = LevelState::STOPPING;
		// reset the selected entity if it was selected from the playing level
		if (m_SelectedEntity && m_SelectedEntity.m_xLevel.lock() == m_xCloneLevel)
		{
			m_SelectedEntity = {};
		}
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




	//////////////////////////////////////////////////////////////////////////
	// EditorGizmo

	glm::mat4	EditorGizmo::s_mViewProj = glm::identity<glm::mat4>();
	glm::vec2	EditorGizmo::s_vViewportPos = VEC2_0;
	glm::vec2	EditorGizmo::s_vViewportSize = VEC2_1;


	void EditorGizmo::FilledCircle( const glm::vec2& _vPoint, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "gizmo" );
		ImDrawList* drawList = /*window ? window->DrawList :*/ ImGui::GetBackgroundDrawList();

		glm::vec2 vSSPoint = GetSSPosition( glm::vec3( _vPoint, 0.f ) );

		drawList->AddCircleFilled( vSSPoint, 6.f, ImColor( _vColor.x, _vColor.y, _vColor.z ) );
	}

	void EditorGizmo::Line( const glm::vec3& _v1, const glm::vec3& _v2, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "gizmo" );
		ImDrawList* drawList = /*window ? window->DrawList :*/ ImGui::GetBackgroundDrawList();

		glm::vec2 vSSPoint1 = GetSSPosition( _v1 );
		glm::vec2 vSSPoint2 = GetSSPosition( _v2 );

		drawList->AddLine( vSSPoint1, vSSPoint2, ImColor( _vColor.x, _vColor.y, _vColor.z ), 2.f );

	}
	void EditorGizmo::AABB( const glm::vec3& _vMin, const glm::vec3& _vMax, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "gizmo" );
		ImDrawList* drawList = /*window ? window->DrawList :*/ ImGui::GetBackgroundDrawList();

		glm::vec3 vMinMaxX = glm::vec3( _vMax.x, _vMin.y, _vMin.z );
		glm::vec3 vMinMaxY = glm::vec3( _vMin.x, _vMax.y, _vMin.z );
		glm::vec3 vMinMaxZ = glm::vec3( _vMin.x, _vMin.y, _vMax.z );
		glm::vec3 vMaxMinX = glm::vec3( _vMin.x, _vMax.y, _vMax.z );
		glm::vec3 vMaxMinY = glm::vec3( _vMax.x, _vMin.y, _vMax.z );
		glm::vec3 vMaxMinZ = glm::vec3( _vMax.x, _vMax.y, _vMin.z );

		glm::vec2 vSSMin = GetSSPosition( _vMin );
		glm::vec2 vSSMax = GetSSPosition( _vMax );
		glm::vec2 vSSMinMaxX = GetSSPosition( vMinMaxX );
		glm::vec2 vSSMinMaxY = GetSSPosition( vMinMaxY );
		glm::vec2 vSSMinMaxZ = GetSSPosition( vMinMaxZ );
		glm::vec2 vSSMaxMinX = GetSSPosition( vMaxMinX );
		glm::vec2 vSSMaxMinY = GetSSPosition( vMaxMinY );
		glm::vec2 vSSMaxMinZ = GetSSPosition( vMaxMinZ );

		const float fThickness = 2.f;
		ImColor col( _vColor.x, _vColor.y, _vColor.z );
		// Min X
		drawList->AddLine( vSSMin, vSSMinMaxY, col, fThickness );
		drawList->AddLine( vSSMin, vSSMinMaxZ, col, fThickness );
		drawList->AddLine( vSSMinMaxY, vSSMaxMinX, col, fThickness );
		drawList->AddLine( vSSMinMaxZ, vSSMaxMinX, col, fThickness );
		// Max X
		drawList->AddLine( vSSMinMaxX, vSSMaxMinY, col, fThickness );
		drawList->AddLine( vSSMinMaxX, vSSMaxMinZ, col, fThickness );
		drawList->AddLine( vSSMaxMinY, vSSMax, col, fThickness );
		drawList->AddLine( vSSMaxMinZ, vSSMax, col, fThickness );
		// Min Y
		drawList->AddLine( vSSMin, vSSMinMaxX, col, fThickness );
		drawList->AddLine( vSSMinMaxZ, vSSMaxMinY, col, fThickness );
		// Max Y
		drawList->AddLine( vSSMinMaxY, vSSMaxMinZ, col, fThickness );
		drawList->AddLine( vSSMaxMinX, vSSMax, col, fThickness );
		// Min Z && Max Z already done
	}

	void EditorGizmo::Bezier( const glm::vec2& _v1, const glm::vec2& _v2, const glm::vec2& _v3, const glm::vec2& _v4, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "gizmo" );
		ImDrawList* drawList = /*window ? window->DrawList :*/ ImGui::GetBackgroundDrawList();

		glm::vec2 vSSPoint1 = GetSSPosition( glm::vec3( _v1, 0.f ) );
		glm::vec2 vSSPoint2 = GetSSPosition( glm::vec3( _v2, 0.f ) );
		glm::vec2 vSSPoint3 = GetSSPosition( glm::vec3( _v3, 0.f ) );
		glm::vec2 vSSPoint4 = GetSSPosition( glm::vec3( _v4, 0.f ) );

		drawList->AddBezierCurve( vSSPoint1, vSSPoint2, vSSPoint3, vSSPoint4, ImColor( _vColor.x, _vColor.y, _vColor.z ), 2.f );
	}

	void EditorGizmo::CatmullRom( const glm::vec3& _v1, const glm::vec3& _v2, const glm::vec3& _v3, const glm::vec3& _v4, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "gizmo" );
		ImDrawList* drawList = /*window ? window->DrawList :*/ ImGui::GetBackgroundDrawList();

		glm::vec2 vSSPoint1 = GetSSPosition( _v1 );
		glm::vec2 vSSPoint2 = GetSSPosition( _v2 );
		glm::vec2 vSSPoint3 = GetSSPosition( _v3 );
		glm::vec2 vSSPoint4 = GetSSPosition( _v4 );

		ImGui::PathCatmullCurve( drawList, vSSPoint1, vSSPoint2, vSSPoint3, vSSPoint4 );
	}


	void EditorGizmo::SetViewProjMatrix( const glm::mat4& _mView, const glm::mat4& _mProj )
	{
		s_mViewProj = _mProj * _mView;
	}
	void EditorGizmo::SetViewportPosAndSize( const glm::vec2& _vPos, const glm::vec2& _vSize )
	{
		s_vViewportPos = _vPos;
		s_vViewportSize = _vSize;
	}

	glm::vec2 EditorGizmo::GetSSPosition( const glm::vec3& _vPos )
	{
		glm::vec4 vClipSpace = s_mViewProj * glm::vec4(_vPos, 1.f);
		glm::vec3 vNDCSpace = glm::vec3(vClipSpace.xyz) / vClipSpace.w; // [-1; 1]
		glm::vec2 vScreenSpace = ((glm::vec2( vNDCSpace.xy ) + VEC2_1) / 2.f);  // [0; 1]
		vScreenSpace.y = 1.f - vScreenSpace.y;
		vScreenSpace.x *= s_vViewportSize.x;
		vScreenSpace.y *= s_vViewportSize.y;
		vScreenSpace.x += s_vViewportPos.x;
		vScreenSpace.y += s_vViewportPos.y;

		return vScreenSpace;
	}
}
