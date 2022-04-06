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
#include "Audio/AudioManager.h"
#include "Editor/Console.h"
#include "ECS/Components/ComponentDefinition.h"
#include "ECS/Components/BaseComponents.h"
#include "ECS/Components/Component_Camera.h"
#include "ECS/Components/Component_ParticleSystem.h"
#include "ECS/Components/Component_Transform.h"
#include "ECS/LevelBase.h"
#include "Input/Input.h"
#include "Events/EventDispatcher.h"
#include "Events/EntityEvent.h"
#include "Utils/OSDialogs.h"
#include "Assets/TextureLoader.h"
#include "Assets/SpritesheetLoader.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"


namespace Plop
{
	namespace Private
	{
		static String sNewName; // For renaming
		static SpritesheetHandle hIconSpriteSheet;
		static const ImVec2 vEditorIconSize( 48, 48 );
		static ImGuizmo::OPERATION eGuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		static ImGuizmo::MODE eGuizmoSpace = ImGuizmo::MODE::LOCAL;
	}

#ifndef USE_COMPONENT_MGR
	::MM::EntityEditor<entt::entity>* EditorLayer::s_pENTTEditor = nullptr;
#endif

	EditorLayer::EditorLayer()
	{
#ifndef USE_COMPONENT_MGR
		s_pENTTEditor = NEW ::MM::EntityEditor<entt::entity>;
#endif
		m_xEditorCamera = std::make_shared<EditorCamera>();
	}

	EditorLayer::~EditorLayer()
	{
#ifndef USE_COMPONENT_MGR
		delete s_pENTTEditor;
#endif
	}

	void EditorLayer::OnRegistered()
	{
		EventDispatcher::RegisterListener( this );

		Private::hIconSpriteSheet = AssetLoader::GetSpritesheet( "Plop\\assets\\icons\\editor.ssdef" );

		m_xEditorCamera->Init();
		m_xEditorCamera->SetNear( 0.01f );
		m_xEditorCamera->SetFar( 100.f );
		m_xEditorCamera->SetOrthographic();
		m_xEditorCamera->SetOrthographicSize( 10.f );
	}

	void EditorLayer::OnUnregistered()
	{
		EventDispatcher::UnregisterListener( this );
	}

	void EditorLayer::OnUpdate( TimeStep& _timeStep )
	{
		auto xLevel = Application::GetCurrentLevel().lock();
		if (xLevel)
		{
			FrameBufferPtr xLevelFrameBufer = Renderer::GetFrameBuffer();
			// resize framebuffer
			if (m_vViewportSize.x > 0 && m_vViewportSize.y > 0 &&
				(xLevelFrameBufer->GetWidth() != m_vViewportSize.x || xLevelFrameBufer->GetHeight() != m_vViewportSize.y))
			{
				xLevelFrameBufer->Resize( (U32)m_vViewportSize.x, (U32)m_vViewportSize.y );
				float fRatio = m_vViewportSize.x / m_vViewportSize.y;
				m_xEditorCamera->SetAspectRatio( fRatio );
				if (!xLevel->GetCamera().expired())
					xLevel->GetCamera().lock()->SetAspectRatio( fRatio );
			}


			xLevelFrameBufer->Bind();
			Renderer::Clear();

			if (m_SelectedEntity && m_SelectedEntity.m_xLevel.lock() == Application::GetCurrentLevel().lock() && m_SelectedEntity.HasComponent<Component_Camera>())
			{
				CameraPtr xCamera = m_SelectedEntity.GetComponent<Component_Camera>().xCamera;
				Renderer::PrepareScene(xCamera->GetProjectionMatrix(), xCamera->GetViewMatrix());
			}
			else if (Application::Get()->IsUsingEditorCamera())
			{
				Renderer::PrepareScene( m_xEditorCamera->GetProjectionMatrix(), m_xEditorCamera->GetViewMatrix() );
			}
			else
			{
				ASSERT( !xLevel->GetCamera().expired() );
				CameraPtr xCamera = xLevel->GetCamera().lock();
				Renderer::PrepareScene( xCamera->GetProjectionMatrix(), xCamera->GetViewMatrix() );
			}

			if (m_eLevelState == EditorLayer::LevelState::RUNNING)
			{
				xLevel->Update( _timeStep );
			}
			else if (m_eLevelState == EditorLayer::LevelState::EDITING)
			{
				xLevel->UpdateInEditor( _timeStep );


				if (m_SelectedEntity && m_SelectedEntity.m_xLevel.lock() == Application::GetCurrentLevel().lock())
				{
					if (m_SelectedEntity.HasComponent<Component_ParticleSystem>())
					{
						m_SelectedEntity.GetComponent<Component_ParticleSystem>().Update( _timeStep );
					}

					// focus camera on entity
					if (Input::IsKeyDown( KeyCode::KEY_F ))
					{
						const glm::vec3& vPos = m_SelectedEntity.GetComponent<Component_Transform>().GetWorldPosition();
						// TODO: get the object size
						m_xEditorCamera->FocusCamera( vPos, VEC3_1 );
					}
				}
			}

			Renderer::EndScene();
			Renderer::GetFrameBuffer()->Unbind();
		}


	}

	void EditorLayer::OnImGuiRender( TimeStep& _timeStep )
	{
		ApplicationLayer::OnImGuiRender( _timeStep );


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


			if (ImGui::Begin( "Scene", nullptr, 0 ))
			{
				if (ImGui::IsWindowHovered( ImGuiHoveredFlags_None ))
				{
					m_xEditorCamera->OnUpdate( _timeStep );
					ImGui::CaptureMouseFromApp( false );
				}


				uint64_t texId = Renderer::GetFrameBuffer()->GetColorID();
				const ImVec2 vViewportSize = ImGui::GetContentRegionAvail();
				m_vViewportSize = vViewportSize;
				const glm::vec2 vWindowPos = ImGui::GetWindowPos();
				const glm::vec2 vRegionMin = ImGui::GetWindowContentRegionMin();
				const glm::vec2 vRegionMax = ImGui::GetWindowContentRegionMax();
				const glm::vec2 vMainViewportPos = ImGui::GetMainViewport()->Pos;
				m_vViewportPosMinScreen = { vWindowPos.x + vRegionMin.x,vWindowPos.y + vRegionMin.y };
				m_vViewportPosMaxScreen = { vWindowPos.x + vRegionMax.x,vWindowPos.y + vRegionMax.y };
				m_vViewportPosMinWindow = m_vViewportPosMinScreen - vMainViewportPos;
				m_vViewportPosMaxWindow = m_vViewportPosMaxScreen - vMainViewportPos;
				ImGui::Image( (ImTextureID)texId, vViewportSize, ImVec2(0, 1), ImVec2(1, 0) );

				if (m_eLevelState == LevelState::EDITING ||
					m_eLevelState == LevelState::PAUSED)
				{
					ShowGizmos();
				}
			}
			ImGui::End();

			// we can draw editor windows here

			if (m_bShowImGuiDemo)
				ImGui::ShowDemoWindow( &m_bShowImGuiDemo );

			if (m_bShowAllocations)
				Debug::ShowAllocationsWindow( &m_bShowAllocations );

			if (m_bShowAudioDebug)
				g_pAudioManager->DrawEditorPanel( &m_bShowAudioDebug );

			if (m_bShowCameraSettings)
				m_xEditorCamera->DisplaySettings( m_bShowCameraSettings );

			// TODO set docking to bottom
			Console::Draw();


			if (m_eLevelState == LevelState::EDITING ||
				m_eLevelState == LevelState::PAUSED)
			{
				ShowSceneGraph();

				LevelBaseWeakPtr xCurrentLevel = Application::GetCurrentLevel();
				if (m_SelectedEntity && !xCurrentLevel.expired())
				{
					LevelBasePtr xLevel = xCurrentLevel.lock();
					if (m_SelectedEntity.m_xLevel.lock() == xLevel)
					{
#ifndef USE_COMPONENT_MGR
						if (!s_pENTTEditor->render( xLevel->m_ENTTRegistry, m_SelectedEntity.m_EntityId ))
							m_SelectedEntity = {};
#endif

						// TODO set docking to right
						ImGui::SetNextWindowSizeConstraints( ImVec2( 450, 600 ), ImVec2( -1, -1 ) );
						String sTitle = m_SelectedEntity.GetComponent<Component_Name>().sName.c_str();
						sTitle += "###Entity Editor";
						if (ImGui::Begin(sTitle.c_str()))
						{
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip( "EnTT id:%llu", entt::to_integral( m_SelectedEntity.m_EntityId ) );

							ImGui::Separator();

							m_SelectedEntity.EditorUI();
						}
						ImGui::End();
					}
				}
			}
		}

		ImGui::End(); // Begin("Editor")
	}

	bool EditorLayer::OnEvent( Event& _event )
	{
		if (_event.GetEventType() == EventType::EntityCreatedEvent)
		{
			EntityDestroyedEvent& entityEvent = (EntityDestroyedEvent&)_event;
			entt::entity enttId = entityEvent.entity.m_EntityId;
			m_mapEntityEditorInfo.insert( { enttId, {} } );
		}
		else if (_event.GetEventType() == EventType::EntityDestroyedEvent)
		{
			EntityDestroyedEvent& entityEvent = (EntityDestroyedEvent&)_event;
			if (m_SelectedEntity == entityEvent.entity)
			{
				m_SelectedEntity.Reset();
			}

			m_mapEntityEditorInfo.erase( entityEvent.entity.m_EntityId );
		}

		return false;
	}


	glm::vec2 EditorLayer::GetViewportPosFromScreenPos( const glm::vec2& _vScreenPos, bool _bClamp /*= false*/ ) const
	{
		const glm::vec2 vScreenSize = (glm::vec2)Application::Get()->GetWindow().GetViewportSize();
		const glm::vec2 vViewportMinScreen = m_vViewportPosMinWindow / vScreenSize;
		const glm::vec2 vViewportMaxScreen = m_vViewportPosMaxWindow / vScreenSize;

		glm::vec2 vViewportPos = (_vScreenPos - vViewportMinScreen) / (vViewportMaxScreen - vViewportMinScreen);

		if (_bClamp)
			vViewportPos = glm::clamp( vViewportPos, VEC2_0, VEC2_1 );

		return vViewportPos;
	}


	json EditorLayer::GetJsonEntity( const Entity& _entity )
	{
		json j;
		entt::entity entityID = _entity.m_EntityId;
		entt::registry& reg = _entity.m_xLevel.lock()->m_ENTTRegistry;

#ifndef USE_COMPONENT_MGR
		for (auto& [component_type_id, ci] : s_pENTTEditor->component_infos)
		{
			if (s_pENTTEditor->entityHasComponent( reg, entityID, component_type_id ))
			{
				j[ci.name] = ci.tojson( reg, entityID );
			}
		}
#endif

		return j;
	}

	bool IconButton(const char* _pButton, bool bActive = false)
	{
		if(!bActive)
			ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
		ImGui::PushID( _pButton );

		glm::vec2 vUVMin;
		glm::vec2 vUVMax;
		bool bClicked = false;
		if (Private::hIconSpriteSheet->GetSpriteUV( _pButton, vUVMin, vUVMax ))
		{
			bClicked = ImGui::ImageButton( (ImTextureID)Private::hIconSpriteSheet->GetNativeHandle(), Private::vEditorIconSize, ImVec2( vUVMin.x, vUVMax.y ), ImVec2( vUVMax.x, vUVMin.y ), 0 );
		}

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
					if (ImGui::MenuItem( "Save ...", "Ctrl + S", nullptr, !Application::GetCurrentLevel().expired() ))
						SaveLevelAs();
				}
				else
				{
					if (ImGui::MenuItem( "Save level", "Ctrl + S", nullptr, !Application::GetCurrentLevel().expired() ))
						SaveLevel();

					if (ImGui::MenuItem( "Save as ...", "Ctrl + Shift + S", nullptr, !Application::GetCurrentLevel().expired() ))
						SaveLevelAs();
				}

				ImGui::Separator();
				if (ImGui::MenuItem( "Exit" ))
					Application::Get()->Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu( "Editor" ))
			{
				ImGui::MenuItem( "Show camera settings", nullptr, &m_bShowCameraSettings );

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
				ImGui::MenuItem( "Show audio debug", nullptr, &m_bShowAudioDebug );

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
			if (Input::IsKeyPressed( KeyCode::KEY_S ) && !Application::GetCurrentLevel().expired())
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
				Private::sNewName = m_SelectedEntity.GetComponent<Component_Name>().sName;
				m_eEditMode = EditMode::RENAMING_ENTITY;
			}
		}
	}

	void EditorLayer::ShowSceneGraph()
	{
		ImGui::SetNextWindowSizeConstraints( ImVec2(400, 500), ImVec2(4000, 5000) );
		if (ImGui::Begin( "Scene graph" ))
		{
			LevelBaseWeakPtr xCurrentLevel = Application::GetCurrentLevel();
			if (!xCurrentLevel.expired())
			{
				LevelBasePtr xLevel = xCurrentLevel.lock();
				const auto& registry = xLevel->m_ENTTRegistry;


				static Entity entityToDestroy;
				static std::function<void( Entity& )> DrawEntity;
				DrawEntity = [this, &registry]( Entity& _Entity ) {

					ImGui::PushID( entt::to_integral( _Entity.m_EntityId ) );

					auto& itEntityInfo = m_mapEntityEditorInfo.find( _Entity.m_EntityId );
					ASSERT( itEntityInfo != m_mapEntityEditorInfo.end() );

					bool bSelected = _Entity == m_SelectedEntity;
					bool bOpen = itEntityInfo->second.bHierarchyOpen;

					const auto& graphNodeParent = registry.get<Component_GraphNode>( _Entity.m_EntityId );
					bool bHasChildren = graphNodeParent.firstChild != entt::null;

					if (bHasChildren)
					{
						ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
						if (ImGui::ArrowButton( "Hierarchy", bOpen ? ImGuiDir_Down : ImGuiDir_Right ))
						{
							bOpen = !bOpen;
							itEntityInfo->second.bHierarchyOpen = bOpen;
						}
						ImGui::PopStyleColor();
					}
					else
					{
						float fSize = ImGui::GetFrameHeight();
						ImGui::Dummy( ImVec2( fSize, fSize ) );
					}

					ImGui::SameLine();


					String& sName = _Entity.GetComponent<Component_Name>().sName;
					if (m_eEditMode == EditMode::RENAMING_ENTITY && bSelected)
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
						if (ImGui::Selectable( sName.c_str(), bSelected ))
						{
							m_SelectedEntity = _Entity;
							m_eEditMode = EditMode::NONE;
						}
					}

					if (ImGui::BeginPopupContextItem( "EntityContextMenu" ))
					{
						if (ImGui::Selectable( "New entity" ))
						{
							LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
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

					if (bHasChildren && bOpen)
					{
						ImGui::Indent();
						_Entity.ChildVisitor( DrawEntity );
						ImGui::Unindent();
					}
				};

				ImGui::BeginChild( "GraphNodeList", ImVec2( 0, -ImGui::GetFrameHeightWithSpacing() ) );

				auto& view = xLevel->m_ENTTRegistry.view<Component_Name>();
				for (auto& e : view)
				{
					Entity entity{ e, Application::GetCurrentLevel()};
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
		ImVec2 vViewportPosition = m_vViewportPosMinScreen;
		ImVec2 vViewportSize = m_vViewportPosMaxScreen - m_vViewportPosMinScreen;

		ImGuizmo::SetRect( vViewportPosition.x, vViewportPosition.y, vViewportSize.x, vViewportSize.y );
		
		EditorGizmo::SetViewportPosAndSize( vViewportPosition, vViewportSize );
#else
		ImVec2 vSize = ImGui::GetIO().DisplaySize;
		ImGuizmo::SetRect( 0, 0, vSize.x, vSize.y );
#endif
		ImGuizmo::SetDrawlist( /*ImGui::GetBackgroundDrawList()*/ );


		// get the current camera, TODO editor camera
		CameraPtr xCurrentCamera = nullptr;
		glm::mat4 mViewMatrix = glm::identity<glm::mat4>();
		glm::mat4 mProjMatrix = glm::identity<glm::mat4>();
		bool bOrthographic = false;
		LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
		if (xLevel)
		{
			mViewMatrix = m_xEditorCamera->GetViewMatrix();
			mProjMatrix = m_xEditorCamera->GetProjectionMatrix();
			bOrthographic = m_xEditorCamera->IsOrthographic();


			if (m_SelectedEntity && m_SelectedEntity.m_xLevel.lock() == Application::GetCurrentLevel().lock())
			{
				if (m_SelectedEntity.HasComponent<Component_Camera>())
				{
					CameraPtr xCamera = m_SelectedEntity.GetComponent<Component_Camera>().xCamera;
					mViewMatrix = xCamera->GetViewMatrix();
					mProjMatrix = xCamera->GetProjectionMatrix();
					bOrthographic = xCamera->IsOrthographic();
				}

				ImGuizmo::SetOrthographic( bOrthographic );
				glm::mat4 mTransform = m_SelectedEntity.GetComponent<Component_Transform>().GetWorldMatrix();

				if (ImGuizmo::Manipulate( glm::value_ptr( mViewMatrix ), glm::value_ptr( mProjMatrix ),
					Private::eGuizmoOperation, Private::eGuizmoSpace, glm::value_ptr( mTransform ) ))
				{
					m_SelectedEntity.GetComponent<Component_Transform>().SetWorldMatrix( mTransform );
				}
			}

			if (true) // TODO: test if editor camera is the currently active one
			{
				// TODO: Position the view square correctly with the panels and fixed offsets from the top and right
				const float fXMul = 0.703f;
				const float fYMul = 0.1f;

#ifdef IMGUI_HAS_VIEWPORT
				float fCornerOffset = 12.f;
				ImVec2 vSize( 100, 100 );
				/*
				ImVec2 vPosition = ImGui::GetMainViewport()->Size;
				vPosition.x *= fXMul;
				vPosition.y *= fYMul;
				vPosition.x += ImGui::GetMainViewport()->Pos.x;
				vPosition.y += ImGui::GetMainViewport()->Pos.y;
				*/
				ImVec2 vPosition = { vViewportPosition.x + vViewportSize.x - fCornerOffset - vSize.x, vViewportPosition.y + fCornerOffset };
#else
				ImVec2 vPosition = io.DisplaySize;
				vPosition.x *= fXMul;
				vPosition.y *= fYMul;
#endif
				if (ImGuizmo::ViewManipulate( glm::value_ptr( mViewMatrix ), m_xEditorCamera->GetTargetDistance(), vPosition, vSize, 0x10101010 ))
				{
					m_xEditorCamera->ChangeView( mViewMatrix );
				}
			}


			EditorGizmo::SetViewProjMatrix( mViewMatrix, mProjMatrix );
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
		m_sCurrentLevel.clear();
	}

	void EditorLayer::OpenLevel()
	{
		StringPath sLevelPath;
		if (Dialog::OpenFile( sLevelPath, Dialog::LEVEL_FILTER ))
		{
			auto xLevel = Application::Get()->CreateNewLevel();
			m_SelectedEntity.Reset();
			xLevel->Load( sLevelPath );
			m_sCurrentLevel = sLevelPath;
		}
	}

	void EditorLayer::SaveLevel()
	{
		if(!m_sCurrentLevel.empty())
			Application::GetCurrentLevel().lock()->Save( m_sCurrentLevel );
	}

	void EditorLayer::SaveLevelAs()
	{
		StringPath sLevelPath;
		if (Dialog::SaveFile( sLevelPath, Dialog::LEVEL_FILTER ))
		{
			// check for extension
			if (sLevelPath.extension() != ".lvl")
				sLevelPath += ".lvl";


			Application::GetCurrentLevel().lock()->Save( sLevelPath );
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
		if (m_SelectedEntity && m_SelectedEntity.m_xLevel.lock() != m_xEditingLevel)
		{
			m_SelectedEntity = {};
		}
	}

	Entity EditorLayer::DuplicateEntity( const Entity& _entity )
	{
		LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
		const String& sName = _entity.GetComponent<Component_Name>().sName;
		Entity dupEntity = xLevel->CreateEntity( sName );
		dupEntity.SetParent( _entity.GetParent() );

		entt::registry& reg = xLevel->m_ENTTRegistry;


#ifdef USE_COMPONENT_MGR
		ComponentManager::DuplicateComponent( reg, _entity.m_EntityId, dupEntity.m_EntityId );
#else
		for (auto& [component_type_id, ci] : s_pENTTEditor->component_infos)
		{
			ci.duplicate( reg, _entity, dupEntity );
		}
#endif

		_entity.ChildVisitor( [&dupEntity](Entity _child ) {

			Entity dupChild = DuplicateEntity( _child );
			dupChild.SetParent( dupEntity );
		} );

		return dupEntity;
	}




	//////////////////////////////////////////////////////////////////////////
	// EditorGizmo

	glm::mat4	EditorGizmo::s_mViewProj = glm::identity<glm::mat4>();
	glm::vec2	EditorGizmo::s_vViewportPos = VEC2_0;
	glm::vec2	EditorGizmo::s_vViewportSize = VEC2_1;


	void EditorGizmo::FilledCircle( const glm::vec2& _vPoint, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "Scene" ); // TODO: handle multiple scene viewport
		ImDrawList* drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect( window->Rect().Min, window->Rect().Max );

		glm::vec2 vSSPoint = GetSSPosition( glm::vec3( _vPoint, 0.f ) );

		drawList->AddCircleFilled( vSSPoint, 6.f, ImColor( _vColor.x, _vColor.y, _vColor.z ) );

		if (window)
			drawList->PopClipRect();
	}

	void EditorGizmo::Line( const glm::vec3& _v1, const glm::vec3& _v2, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "Scene" ); // TODO: handle multiple scene viewport
		ImDrawList* drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect( window->Rect().Min, window->Rect().Max );

		glm::vec2 vSSPoint1 = GetSSPosition( _v1 );
		glm::vec2 vSSPoint2 = GetSSPosition( _v2 );

		drawList->AddLine( vSSPoint1, vSSPoint2, ImColor( _vColor.x, _vColor.y, _vColor.z ), 2.f );

		if (window)
			drawList->PopClipRect();
	}

	void EditorGizmo::AABB( const glm::vec3& _vMin, const glm::vec3& _vMax, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "Scene" ); // TODO: handle multiple scene viewport
		ImDrawList* drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect( window->Rect().Min, window->Rect().Max );

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
		
		if (window)
			drawList->PopClipRect();
	}

	void EditorGizmo::Bezier( const glm::vec2& _v1, const glm::vec2& _v2, const glm::vec2& _v3, const glm::vec2& _v4, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "Scene" ); // TODO: handle multiple scene viewport
		ImDrawList* drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect( window->Rect().Min, window->Rect().Max );

		glm::vec2 vSSPoint1 = GetSSPosition( glm::vec3( _v1, 0.f ) );
		glm::vec2 vSSPoint2 = GetSSPosition( glm::vec3( _v2, 0.f ) );
		glm::vec2 vSSPoint3 = GetSSPosition( glm::vec3( _v3, 0.f ) );
		glm::vec2 vSSPoint4 = GetSSPosition( glm::vec3( _v4, 0.f ) );

		drawList->AddBezierCurve( vSSPoint1, vSSPoint2, vSSPoint3, vSSPoint4, ImColor( _vColor.x, _vColor.y, _vColor.z ), 2.f );

		if (window)
			drawList->PopClipRect();
	}

	void EditorGizmo::CatmullRom( const glm::vec3& _v1, const glm::vec3& _v2, const glm::vec3& _v3, const glm::vec3& _v4, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "Scene" ); // TODO: handle multiple scene viewport
		ImDrawList* drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect( window->Rect().Min, window->Rect().Max );

		glm::vec2 vSSPoint1 = GetSSPosition( _v1 );
		glm::vec2 vSSPoint2 = GetSSPosition( _v2 );
		glm::vec2 vSSPoint3 = GetSSPosition( _v3 );
		glm::vec2 vSSPoint4 = GetSSPosition( _v4 );

		ImGui::PathCatmullCurve( drawList, vSSPoint1, vSSPoint2, vSSPoint3, vSSPoint4 );
		
		if (window)
			drawList->PopClipRect();
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
