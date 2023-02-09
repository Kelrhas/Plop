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
#include "ECS/Components/ComponentDefinition.h"
#include "ECS/Components/BaseComponents.h"
#include "ECS/Components/Component_Camera.h"
#include "ECS/Components/Component_ParticleSystem.h"
#include "ECS/Components/Component_PrefabInstance.h"
#include "ECS/Components/Component_Transform.h"
#include "ECS/Components/Component_SpriteRenderer.h"
#include "ECS/LevelBase.h"
#include "ECS/Serialisation.h"
#include "Editor/UndoManager.h"
#include "Editor/EditorStyle.h"
#include "ECS/PrefabManager.h"
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
		static const ImVec2 vToolbarIconSize( 48, 48 );
		static const ImVec2 vEntityIconSize( 20, 20 );
		static ImGuizmo::OPERATION eGuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		static ImGuizmo::MODE eGuizmoSpace = ImGuizmo::MODE::LOCAL;

		enum class CopyType : U8
		{
			NONE,
			ENTITY,
		};
	}


	EditorLayer::EditorLayer()
	{
		m_xEditorCamera = std::make_shared<EditorCamera>();



		UndoManager::RegisterActionCommands(UndoAction::Type::ENTITY_MOVE,
			[this] (const UndoAction::UndoData& _data) -> bool
			{
				if (m_xEditingLevel->GetEntityRegistry().valid(_data.entityVec3.enttID))
				{
					auto& transformComp = m_xEditingLevel->GetEntityRegistry().get<Component_Transform>(_data.entityVec3.enttID);
					transformComp.SetLocalPosition(_data.entityVec3.vOld);
					return true;
				}
				return false;
			},
			[this] (const UndoAction::UndoData& _data) -> bool
			{
				if (m_xEditingLevel->GetEntityRegistry().valid(_data.entityVec3.enttID))
				{
					auto& transformComp = m_xEditingLevel->GetEntityRegistry().get<Component_Transform>(_data.entityVec3.enttID);
					transformComp.SetLocalPosition(_data.entityVec3.vNew);
					return true;
				}
				return false;
			});

		UndoManager::RegisterActionCommands(UndoAction::Type::ENTITY_ROTATE,
			[this] (const UndoAction::UndoData& _data) -> bool
			{
				if (m_xEditingLevel->GetEntityRegistry().valid(_data.entityQuat.enttID))
				{
					auto& transformComp = m_xEditingLevel->GetEntityRegistry().get<Component_Transform>(_data.entityQuat.enttID);
					transformComp.SetLocalRotation(_data.entityQuat.qOld);
					return true;
				}
				return false;
			},
			[this] (const UndoAction::UndoData& _data) -> bool
			{
				if (m_xEditingLevel->GetEntityRegistry().valid(_data.entityQuat.enttID))
				{
					auto& transformComp = m_xEditingLevel->GetEntityRegistry().get<Component_Transform>(_data.entityQuat.enttID);
					transformComp.SetLocalRotation(_data.entityQuat.qNew);
					return true;
				}
				return false;
			});

		UndoManager::RegisterActionCommands(UndoAction::Type::ENTITY_SCALE,
			[this] (const UndoAction::UndoData& _data) -> bool
			{
				if (m_xEditingLevel->GetEntityRegistry().valid(_data.entityVec3.enttID))
				{
					auto& transformComp = m_xEditingLevel->GetEntityRegistry().get<Component_Transform>(_data.entityVec3.enttID);
					transformComp.SetLocalScale(_data.entityVec3.vOld);
					return true;
				}
				return false;
			},
			[this] (const UndoAction::UndoData& _data) -> bool
			{
				if (m_xEditingLevel->GetEntityRegistry().valid(_data.entityVec3.enttID))
				{
					auto& transformComp = m_xEditingLevel->GetEntityRegistry().get<Component_Transform>(_data.entityVec3.enttID);
					transformComp.SetLocalScale(_data.entityVec3.vNew);
					return true;
				}
				return false;
			});

		UndoManager::RegisterActionCommands(UndoAction::Type::ENTITY_GIZMO_MANIPULATE,
			[this] (const UndoAction::UndoData& _data) -> bool
			{
				if (m_xEditingLevel->GetEntityRegistry().valid(_data.entityMat4.enttID))
				{
					auto& transformComp = m_xEditingLevel->GetEntityRegistry().get<Component_Transform>(_data.entityMat4.enttID);
					transformComp.SetWorldMatrix(_data.entityMat4.mOld);
					return true;
				}
				return false;
			},
			[this] (const UndoAction::UndoData& _data) -> bool
			{
				if (m_xEditingLevel->GetEntityRegistry().valid(_data.entityMat4.enttID))
				{
					auto& transformComp = m_xEditingLevel->GetEntityRegistry().get<Component_Transform>(_data.entityMat4.enttID);
					transformComp.SetWorldMatrix(_data.entityMat4.mNew);
					return true;
				}
				return false;
			});

		UndoManager::RegisterActionCommands(UndoAction::Type::ENTITY_VISIBILITY,
			[this] (const UndoAction::UndoData& _data) -> bool
			{
				if (m_xEditingLevel->GetEntityRegistry().valid(_data.entityVisibility.enttID))
				{
					Entity e(_data.entityVisibility.enttID, m_xEditingLevel->GetEntityRegistry());
					e.SetVisible(_data.entityVisibility.bValueBefore);
					return true;
				}
				return false;
			},
			[this] (const UndoAction::UndoData& _data) -> bool
			{
				if (m_xEditingLevel->GetEntityRegistry().valid(_data.entityVisibility.enttID))
				{
					Entity e(_data.entityVisibility.enttID, m_xEditingLevel->GetEntityRegistry());
					e.SetVisible(_data.entityVisibility.bValueAfter);
					return true;
				}
				return false;
			});

	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnRegistered()
	{
		EventDispatcher::RegisterListener( this );

		Private::hIconSpriteSheet = AssetLoader::GetSpritesheet( Application::Get()->GetEditorDirectory() / "assets/icons/editor.ssdef" );

		m_xEditorCamera->Init();
		m_xEditorCamera->SetNear( 0.01f );
		m_xEditorCamera->SetFar( 100.f );
		m_xEditorCamera->SetOrthographic();
		m_xEditorCamera->SetOrthographicSize( 10.f );
	}

	void EditorLayer::OnUnregistered()
	{
		m_xEditingLevel->Shutdown();
		m_xEditingLevel.reset();

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

			CameraPtr xCamera;
			if (IsSelectedEntityForLevel(Application::GetCurrentLevel()) && m_SelectedEntity.HasComponent<Component_Camera>())
			{
				xCamera = m_SelectedEntity.GetComponent<Component_Camera>().xCamera;
				if (!xCamera && Application::Get()->IsUsingEditorCamera())
					xCamera = m_xEditorCamera;
			}
			else if (Application::Get()->IsUsingEditorCamera())
			{
				xCamera = m_xEditorCamera;
			}
			else
			{
				xCamera = xLevel->GetCamera().lock();
			}

			if (xCamera)
				Renderer::PrepareScene(xCamera->GetProjectionMatrix(), xCamera->GetViewMatrix());

			if (m_eLevelState == EditorLayer::LevelState::RUNNING)
			{
				xLevel->Update(_timeStep);
			}
			else if (m_eLevelState == EditorLayer::LevelState::EDITING || m_eLevelState == EditorLayer::LevelState::PAUSED)
			{
				xLevel->UpdateInEditor(_timeStep);


				if (IsSelectedEntityForLevel(Application::GetCurrentLevel()))
				{
					if (IsEditing() && m_SelectedEntity.HasComponent<Component_ParticleSystem>())
					{
						m_SelectedEntity.GetComponent<Component_ParticleSystem>().Update(_timeStep);
					}

					// focus camera on entity
					if (Input::IsKeyDown(KeyCode::KEY_F))
					{
						const glm::vec3 &vPos = m_SelectedEntity.GetComponent<Component_Transform>().GetWorldPosition();
						// TODO: get the object size
						m_xEditorCamera->FocusCamera(vPos, VEC3_1);
					}
				}
			}

			if (xCamera)
			{
				Renderer::EndScene();
				Renderer::GetFrameBuffer()->Unbind();
			}
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


			if (ImGui::Begin( "Scene", nullptr, ImGuiWindowFlags_None))
			{
				if (ImGui::IsWindowHovered( ImGuiHoveredFlags_None ))
				{
					m_xEditorCamera->OnUpdate( _timeStep );
					ImGui::CaptureMouseFromApp( false );
				}


				uint64_t texId = Renderer::GetFrameBuffer()->GetColorID(0);
				const ImVec2 vViewportSize = ImGui::GetContentRegionAvail();
				m_vViewportSize = vViewportSize;
				const glm::vec2 vImguiWindowPos = ImGui::GetWindowPos();
				const glm::vec2 vRegionMin = ImGui::GetWindowContentRegionMin();
				const glm::vec2 vRegionMax = ImGui::GetWindowContentRegionMax();
				const glm::vec2 vWindowViewportPos = ImGui::GetCurrentWindow()->Viewport->Pos;
				m_vViewportPosMinScreenSpace = {vImguiWindowPos.x + vRegionMin.x, vImguiWindowPos.y + vRegionMin.y};
				m_vViewportPosMaxScreenSpace = {vImguiWindowPos.x + vRegionMax.x, vImguiWindowPos.y + vRegionMax.y};
				m_vViewportPosMinWindowSpace = vRegionMin + vImguiWindowPos - vWindowViewportPos; //m_vViewportPosMinScreenSpace - vMainViewportPos;
				m_vViewportPosMaxWindowSpace = vRegionMax + vImguiWindowPos - vWindowViewportPos; //m_vViewportPosMaxScreenSpace - vMainViewportPos;

				{
					const ImVec2 vUvMin = Renderer::USE_INVERTED_Y_UV ? ImVec2(0, 1) : ImVec2(0, 0);
					const ImVec2 vUvMax = Renderer::USE_INVERTED_Y_UV ? ImVec2(1, 0) : ImVec2(1, 1);
					ImGui::Image((ImTextureID)texId, vViewportSize, vUvMin, vUvMax);
				}

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

			if (m_eLevelState == LevelState::EDITING ||
				m_eLevelState == LevelState::PAUSED)
			{
				PrefabManager::ImGuiRenderLibraries();

				ShowSceneGraph();

				LevelBaseWeakPtr xCurrentLevel = Application::GetCurrentLevel();
				if (!xCurrentLevel.expired())
				{
					LevelBasePtr xLevel = xCurrentLevel.lock();
					bool bValidEntity = IsSelectedEntityForLevel(xLevel);

					// TODO set docking to right
					ImGui::SetNextWindowSizeConstraints( ImVec2( 450, 600 ), ImVec2( -1, -1 ) );
					String sTitle = bValidEntity ? m_SelectedEntity.GetComponent<Component_Name>().sName.c_str() : "---";
					sTitle += "###Entity Editor";
					if (ImGui::Begin(sTitle.c_str()))
					{
						if (bValidEntity)
						{
							if (ImGui::IsItemHovered())
							{
								ImGui::SetTooltip("GUID: %llu\nEnTT id:%llu", m_SelectedEntity.GetComponent<Component_Name>().guid, entt::to_integral(m_SelectedEntity.m_hEntity.entity()));
							}

							ImGui::Separator();

							m_SelectedEntity.EditorUI();
						}
					}
					ImGui::End();
				}
			}
		}

		ImGui::End(); // Begin("Editor")
	}

	bool EditorLayer::OnEvent(Event *_pEvent)
	{
		if (_pEvent->GetEventType() == EventType::EntityCreatedEvent)
		{
			EntityCreatedEvent &entityEvent = *(EntityCreatedEvent *)_pEvent;
			entt::entity enttId = entityEvent.entity;
			if (IsUsingEditorLevel())
				m_mapEntityEditorInfoEditing.insert({ enttId, {} });
			else
				m_mapEntityEditorInfoPlaying.insert({ enttId, {} });
		}
		else if (_pEvent->GetEventType() == EventType::EntityDestroyedEvent)
		{
			EntityDestroyedEvent &entityEvent = *(EntityDestroyedEvent *)_pEvent;
			if (m_SelectedEntity == entityEvent.entity)
			{
				m_SelectedEntity.Reset();
			}

			if (IsUsingEditorLevel())
				m_mapEntityEditorInfoEditing.erase((entt::entity)entityEvent.entity);
			else
				m_mapEntityEditorInfoPlaying.erase((entt::entity)entityEvent.entity);
		}
		else if (_pEvent->GetEventType() == EventType::PrefabInstantiatedEvent)
		{
			PrefabInstantiatedEvent &entityEvent = *(PrefabInstantiatedEvent *)_pEvent;
			std::stack<entt::entity> todo;
			todo.push(entityEvent.entity);

			const auto &reg = entityEvent.entity.m_hEntity.registry();

			while (!todo.empty())
			{
				entt::entity enttID = todo.top();
				todo.pop();

				if (IsUsingEditorLevel())
					m_mapEntityEditorInfoEditing.insert({ enttID, {} });
				else
					m_mapEntityEditorInfoPlaying.insert({ enttID, {} });

				auto &graphComp = reg.get<Component_GraphNode>(enttID);
				if (graphComp.firstChild != entt::null)
					todo.push(graphComp.firstChild);
				if (graphComp.nextSibling != entt::null)
					todo.push(graphComp.nextSibling);
			}
		}

		return false;
	}


	glm::vec2 EditorLayer::GetViewportPosFromWindowPos( const glm::vec2& _vScreenPos, bool _bClamp /*= false*/ ) const
	{
		ASSERTM(_vScreenPos.x <= 1.f && _vScreenPos.y <= 1.f, "Should be normalized values");

		const glm::vec2 vWindowSize = (glm::vec2)Application::Get()->GetWindow().GetViewportSize();
		const glm::vec2 vViewportMinScreen = m_vViewportPosMinWindowSpace / vWindowSize;
		const glm::vec2 vViewportMaxScreen = m_vViewportPosMaxWindowSpace / vWindowSize;

		glm::vec2 vViewportPos = (_vScreenPos - vViewportMinScreen) / (vViewportMaxScreen - vViewportMinScreen);

		if (_bClamp)
			vViewportPos = glm::clamp( vViewportPos, VEC2_0, VEC2_1 );

		return vViewportPos;
	}


	json EditorLayer::GetJsonEntity( const Entity& _entity )
	{
		json j;

		return j;
	}

	bool IconButton(const char* _pButton, ImVec2 _vSize, bool _bActive = false)
	{
		if(!_bActive)
			ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
		ImGui::PushID( _pButton );

		glm::vec2 vUVMin;
		glm::vec2 vUVMax;
		bool bClicked = false;
		if (Private::hIconSpriteSheet->GetSpriteUV( _pButton, vUVMin, vUVMax ))
		{
			bClicked = ImGui::ImageButton((ImTextureID)Private::hIconSpriteSheet->GetNativeHandle(), _vSize, vUVMin, vUVMax, 0);
		}

		ImGui::PopID();
		if (!_bActive)
			ImGui::PopStyleColor();

		return bClicked;
	}

	void Icon(const char *_pButton, ImVec2 _vSize)
	{
		glm::vec2 vUVMin;
		glm::vec2 vUVMax;
		bool	  bClicked = false;
		if (Private::hIconSpriteSheet->GetSpriteUV(_pButton, vUVMin, vUVMax))
		{
			ImGui::Image((ImTextureID)Private::hIconSpriteSheet->GetNativeHandle(), _vSize, vUVMin, vUVMax);
		}
	}


	void EditorLayer::ShowToolBar()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		float fBarHeight = Private::vToolbarIconSize.y + 2 * ImGui::GetStyle().WindowPadding.y;
		ImGui::SetNextWindowPos( viewport->GetWorkPos() );
		ImGui::SetNextWindowSize( ImVec2( viewport->GetWorkSize().x, fBarHeight ) );
		ImGui::SetNextWindowViewport( viewport->ID );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.f );
		if (ImGui::Begin( "##Manipulation", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize ))
		{
			ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

			if (IconButton("Translate", Private::vToolbarIconSize, Private::eGuizmoOperation == ImGuizmo::OPERATION::TRANSLATE))
				Private::eGuizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

			ImGui::SameLine();
			if (IconButton("Rotate", Private::vToolbarIconSize, Private::eGuizmoOperation == ImGuizmo::OPERATION::ROTATE))
				Private::eGuizmoOperation = ImGuizmo::OPERATION::ROTATE;

			ImGui::SameLine();
			if (IconButton("Scale", Private::vToolbarIconSize, Private::eGuizmoOperation == ImGuizmo::OPERATION::SCALE))
				Private::eGuizmoOperation = ImGuizmo::OPERATION::SCALE;





			ImGui::SameLine(0, 60);
			ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, Private::vToolbarIconSize.y / 4.f );
			{
				bool bWasLocal = Private::eGuizmoSpace == ImGuizmo::MODE::LOCAL;
				if (!bWasLocal)
					ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );

				if (ImGui::ButtonRounded( "Local", ImVec2( 0, Private::vToolbarIconSize.y ), ImDrawCornerFlags_Left ))
					Private::eGuizmoSpace = ImGuizmo::MODE::LOCAL;

				if (!bWasLocal)
					ImGui::PopStyleColor();
				else
					ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );

				ImGui::SameLine();
				if (ImGui::ButtonRounded( "World", ImVec2( 0, Private::vToolbarIconSize.y ), ImDrawCornerFlags_Right ))
					Private::eGuizmoSpace = ImGuizmo::MODE::WORLD;

				if(bWasLocal)
					ImGui::PopStyleColor(); // ImGuiCol_Button
			}
			ImGui::PopStyleVar(); // ImGuiStyleVar_FrameRounding




			ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing

			ImGui::SameLine( 0, 60 );
			if (m_eLevelState == LevelState::RUNNING)
			{
				if (IconButton("Pause", Private::vToolbarIconSize))
					PauseLevel();

				ImGui::SameLine();
				if (IconButton("Stop", Private::vToolbarIconSize))
					StopLevel();
			}
			else if (m_eLevelState == LevelState::PAUSED)
			{
				if (IconButton("Play", Private::vToolbarIconSize))
					ResumeLevel();

				ImGui::SameLine();
				if (IconButton("Stop", Private::vToolbarIconSize))
					StopLevel();
			}
			else if (m_eLevelState == LevelState::EDITING)
			{
				if (IconButton("Play", Private::vToolbarIconSize))
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
				if (ImGui::MenuItem("Undo", "CTRL + Z", nullptr, UndoManager::CanUndo()))
					UndoManager::Undo();
				if (ImGui::MenuItem("Redo", "CTRL + Y", nullptr, UndoManager::CanRedo()))
					UndoManager::Redo();
				ImGui::MenuItem("Show camera settings", nullptr, &m_bShowCameraSettings);

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
			if (m_SelectedEntity)
			{
				if (Input::IsKeyPressed(KeyCode::KEY_D))
				{
					DuplicateEntity(m_SelectedEntity);
				}
				if (Input::IsKeyPressed(KeyCode::KEY_C))
				{
					CopyEntity(m_SelectedEntity);
				}
				if (Input::IsKeyPressed(KeyCode::KEY_H))
				{
					ToggleEntity(m_SelectedEntity);
				}
			}
			if (Input::IsKeyPressed(KeyCode::KEY_V) && CanPasteEntity(m_SelectedEntity))
			{
				PasteEntity(m_SelectedEntity);
			}

			if (Input::IsKeyPressed(KeyCode::KEY_Z))
			{
				UndoManager::Undo();
			}
			if (Input::IsKeyPressed(KeyCode::KEY_Y))
			{
				UndoManager::Redo();
			}

		}

		if (m_SelectedEntity)
		{
			if (m_eEditMode == EditMode::NONE)
			{
				if (Input::IsKeyPressed(KeyCode::KEY_F2))
				{
					Private::sNewName = m_SelectedEntity.GetComponent<Component_Name>().sName;
					m_eEditMode = EditMode::RENAMING_ENTITY;
				}

				if (Input::IsKeyPressed(KeyCode::KEY_Delete))
				{
					Application::GetCurrentLevel().lock()->DestroyEntity(m_SelectedEntity);
					m_SelectedEntity.Reset();
				}
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

					EntityEditorInfoMap &entityInfoMap = (IsUsingEditorLevel() ? m_mapEntityEditorInfoEditing : m_mapEntityEditorInfoPlaying);
					auto				 itEntityInfo  = entityInfoMap.find(_Entity);
					ASSERT(itEntityInfo != entityInfoMap.end());
					if (itEntityInfo == entityInfoMap.end())
					{
						ImGui::TextColored(ImColor(255, 0, 0), _Entity.GetComponent<Component_Name>().sName.c_str());
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("error on entityInfoMap");
						return;
					}

					ImGui::PushID((int)entt::to_integral(_Entity.m_hEntity.entity()));

					bool bSelected = _Entity == m_SelectedEntity;
					bool bOpen = itEntityInfo->second.bHierarchyOpen;

					const auto& graphComp = registry.get<Component_GraphNode>( _Entity );
					bool bHasChildren = graphComp.firstChild != entt::null;

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

					EditorStyle eStyle = graphComp.uFlags.Has(EntityFlag::HIDE) ? EditorStyle::ENTITY_HIDDEN : EditorStyle::ENTITY_VALID;
					if (_Entity.HasComponent<Component_PrefabInstance>())
					{
						auto &prefabComp = _Entity.GetComponent<Component_PrefabInstance>();
						if (prefabComp.hSrcPrefab)
							eStyle = EditorStyle::PREFAB;
						else
							eStyle = EditorStyle::PREFAB_INVALID;

						Icon("prefab", Private::vEntityIconSize);
						ImGui::SameLine();
					}

					ImGui::PushStyleColor(eStyle);

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
						ImGuiSelectableFlags flags = ImGuiSelectableFlags_None;
						if (!m_bAllowPicking)
							flags |= ImGuiSelectableFlags_Disabled;
						if (ImGui::Selectable( sName.c_str(), bSelected, flags))
						{
							m_SelectedEntity = _Entity;
							m_eEditMode = EditMode::NONE;
						}
					}
					ImGui::PopStyleColor(eStyle);

					if (ImGui::BeginPopupContextItem( "EntityContextMenu" ))
					{
						if (ImGui::MenuItem( "New entity" ))
						{
							LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
							Entity e = xLevel->CreateEntity();
							e.SetParent( _Entity );
						}
						if (ImGui::MenuItem("Duplicate entity", "CTRL + D"))
						{
							DuplicateEntity(_Entity);
						}
						if (ImGui::MenuItem("Copy entity", "CTRL + C"))
						{
							CopyEntity(_Entity);
						}
						if (ImGui::MenuItem("Paste entity", "CTRL + V", nullptr, CanPasteEntity(_Entity)))
						{
							PasteEntity(_Entity);
						}
						if (ImGui::MenuItem("Visible", "CTRL + H", _Entity.IsVisible()))
						{
							ToggleEntity(_Entity);
						}
						if (ImGui::MenuItem( "Delete entity", "Del" ))
							entityToDestroy = _Entity;
						ImGui::Separator();

						if (_Entity.HasComponent<Component_PrefabInstance>())
						{
							if (ImGui::MenuItem("Update prefab from this"))
							{
								auto &comp = _Entity.GetComponent<Component_PrefabInstance>();
								PrefabManager::UpdatePrefabFromInstance(comp.hSrcPrefab, _Entity);
							}
							if (ImGui::MenuItem("Break prefab link"))
							{
								PrefabManager::RemovePrefabReferenceFromInstance(_Entity);
							}
						}
						else if (!PrefabManager::IsPartOfPrefab(_Entity))
						{
							if (ImGui::BeginMenu("Create prefab"))
							{
								if (PrefabManager::HasAnyPrefabLib())
								{
									PrefabManager::VisitAllLibraries([_Entity](const String &_sName, const PrefabLibrary &_lib) {
										if (ImGui::MenuItem(_sName.c_str()))
										{
											PrefabManager::CreatePrefab(_Entity, _sName);
											return VisitorFlow::BREAK;
										}
										return VisitorFlow::CONTINUE;
									});
								}
								else
								{
									ImGui::PushStyleColor(EditorStyle::DISABLED);
									ImGui::Text("no lib available, create one before");
									ImGui::PopStyleColor(EditorStyle::DISABLED);
								}

								ImGui::EndMenu();
							}
						}

						ImGui::EndPopup();
					}
					else
					{
						if (ImGui::BeginDragDropSource( ImGuiDragDropFlags_None ))
						{
							ImGui::SetDragDropPayload( "ReparentEntity", &_Entity.m_hEntity, sizeof( _Entity.m_hEntity ) );
							ImGui::Text( sName.c_str() );
							ImGui::EndDragDropSource();
						}
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload( "ReparentEntity" ))
							{
								ASSERTM(pPayload->DataSize == sizeof(entt::handle), "Wrong Drag&Drop payload");
								Entity child( *((entt::handle*)pPayload->Data) );

								if (child.GetParent() != _Entity)
									child.SetParent( _Entity );
								else
									child.SetParent( Entity() );
							}
							else if (const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload( "InstantiatePrefab" ))
							{
								ASSERTM( pPayload->DataSize == sizeof( GUID ), "Wrong Drag&Drop payload" );
								PrefabManager::InstantiatePrefab(*(GUID*)pPayload->Data, _Entity.m_hEntity.registry(), _Entity);
							}
							ImGui::EndDragDropTarget();
						}
					}

					ImGui::PopID();

					if (bHasChildren && bOpen)
					{
						ImGui::Indent();
						_Entity.VisitChildren([](Entity child) {
							DrawEntity(child);
							return VisitorFlow::CONTINUE;
						});
						ImGui::Unindent();
					}
				};

				ImGui::BeginChild( "GraphNodeList", ImVec2( 0, -ImGui::GetFrameHeightWithSpacing() ) );

				auto view = xLevel->m_ENTTRegistry.view<Component_Name>();
				for (auto& e : view)
				{
					Entity entity = { e, xLevel->m_ENTTRegistry };
					Entity parent = entity.GetParent();

					// only draw those without parent, and each one will draw their children
					if (!parent)
					{
						DrawEntity( entity );
					}
				}

				if (entityToDestroy)
					xLevel->DestroyEntity(entityToDestroy);

				ImGui::EndChild();

				if (ImGui::BeginDragDropTarget())
				{

					if (const ImGuiPayload *pPayload = ImGui::AcceptDragDropPayload("ReparentEntity"))
					{
						ASSERTM(pPayload->DataSize == sizeof(entt::handle), "Wrong Drag&Drop payload");
						Entity child(*((entt::handle *)pPayload->Data));
						child.SetParent(Entity());
					}
					else if (const ImGuiPayload *pPayload = ImGui::AcceptDragDropPayload("InstantiatePrefab"))
					{
						ASSERTM(pPayload->DataSize == sizeof(GUID), "Wrong Drag&Drop payload");
						PrefabManager::InstantiatePrefab(*(GUID *)pPayload->Data, Application::Get()->GetCurrentLevel().lock()->GetEntityRegistry(), entt::null);
					}
					ImGui::EndDragDropTarget();
				}


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
		ImVec2 vViewportPosition = m_vViewportPosMinScreenSpace;
		ImVec2 vViewportSize = m_vViewportPosMaxScreenSpace - m_vViewportPosMinScreenSpace;

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
			xCurrentCamera = m_xEditorCamera;


			if (IsSelectedEntityForLevel(Application::GetCurrentLevel()))
			{
				glm::mat4 mTransform = m_SelectedEntity.GetComponent<Component_Transform>().GetWorldMatrix();

				if (m_SelectedEntity.HasComponent<Component_SpriteRenderer>())
					EditorGizmo::Cube(glm::vec3(0.5f, 0.5f, 0.01f), mTransform);

				if (m_SelectedEntity.HasComponent<Component_Camera>())
				{
					CameraPtr xCamera = m_SelectedEntity.GetComponent<Component_Camera>().xCamera;
					if (xCamera)
					{
						mViewMatrix = xCamera->GetViewMatrix();
						mProjMatrix = xCamera->GetProjectionMatrix();
						bOrthographic = xCamera->IsOrthographic();
						xCurrentCamera = xCamera;
					}
				}

				ImGuizmo::SetOrthographic( bOrthographic );

				if (ImGuizmo::Manipulate( glm::value_ptr( mViewMatrix ), glm::value_ptr( mProjMatrix ),
					Private::eGuizmoOperation, Private::eGuizmoSpace, glm::value_ptr( mTransform ) ))
				{
					m_SelectedEntity.GetComponent<Component_Transform>().SetWorldMatrix( mTransform );
				}


				// Undo
				{
					static glm::mat4 mBackup;
					static bool bUsing = false;
					if (ImGuizmo::IsUsing())
					{
						if (!bUsing)
						{
							mBackup = mTransform;
							bUsing = true;
						}
					}
					else
					{
						if (bUsing)
						{
							UndoManager::PushAction(UndoAction::EntityGizmoManipulate(m_SelectedEntity, mBackup, mTransform));
							bUsing = false;
						}
					}
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


			EditorGizmo::SetCamera( xCurrentCamera );
		}


		if (false)
		{
			static const glm::mat4 mIdentity = glm::identity<glm::mat4>();
			ImGuizmo::DrawGrid( glm::value_ptr( mViewMatrix ), glm::value_ptr( mProjMatrix ), glm::value_ptr( mIdentity ), 10.f );
		}
	}

	void EditorLayer::NewLevel()
	{
		if (m_xEditingLevel)
			m_xEditingLevel->Shutdown();
		if (Application::Get()->m_xLoadedLevel)
		{
			Application::Get()->m_xLoadedLevel->Shutdown();
			Application::Get()->m_xLoadedLevel = nullptr;
		}
		m_xEditingLevel = Application::Get()->CreateNewLevel();
		m_xEditingLevel->Init();
		m_SelectedEntity.Reset();
		m_sCurrentLevel.clear();

		Application::Get()->GetWindow().SetTitleSuffix("*unsaved*");
	}

	void EditorLayer::OpenLevel()
	{
		StringPath sLevelPath;
		if (Dialog::OpenFile( sLevelPath, Dialog::LEVEL_FILTER ))
		{
			if (m_xEditingLevel)
				m_xEditingLevel->Shutdown();
			if (Application::Get()->m_xLoadedLevel)
			{
				Application::Get()->m_xLoadedLevel->Shutdown();
				Application::Get()->m_xLoadedLevel = nullptr;
			}
			m_xEditingLevel = Application::Get()->CreateNewLevel();
			m_SelectedEntity.Reset();
			m_xEditingLevel->Load( sLevelPath );
			m_sCurrentLevel = sLevelPath;

			Application::Get()->GetWindow().SetTitleSuffix(sLevelPath.stem().string());
		}
	}

	void EditorLayer::SaveLevel()
	{
		if (!m_sCurrentLevel.empty())
		{
			m_xEditingLevel->Save(m_sCurrentLevel);
			PrefabManager::SaveLibraries();
		}
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
			PrefabManager::SaveLibraries();
			m_sCurrentLevel = sLevelPath;

			Application::Get()->GetWindow().SetTitleSuffix(sLevelPath.stem().string());
		}
	}

	void EditorLayer::PlayLevel()
	{
		m_eLevelState = LevelState::STARTING;
		ASSERT(m_mapEntityEditorInfoPlaying.empty());
	}

	void EditorLayer::PauseLevel()
	{
		m_eLevelState = LevelState::PAUSED;
		TimeStep::PushGameScale(0.f);
	}

	void EditorLayer::ResumeLevel()
	{
		m_eLevelState = LevelState::RUNNING;
		TimeStep::PopGameScale();
	}

	void EditorLayer::StopLevel()
	{
		if (m_eLevelState == LevelState::PAUSED)
			TimeStep::PopGameScale();

		m_eLevelState = LevelState::STOPPING;
		// reset the selected entity if it was selected from the playing level
		if (!IsSelectedEntityForLevel(m_xEditingLevel))
		{
			m_SelectedEntity = {};
		}
		m_mapEntityEditorInfoPlaying.clear();
	}

	void EditorLayer::UpdateEntityInfo()
	{
		EntityEditorInfoMap &entityInfoMap = (IsUsingEditorLevel() ? m_mapEntityEditorInfoEditing : m_mapEntityEditorInfoPlaying);

		LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
		xLevel->GetEntityRegistry().each([&entityInfoMap](entt::entity _e) { entityInfoMap[_e] = {}; });
	}

	bool EditorLayer::IsSelectedEntityForLevel(LevelBaseWeakPtr _xLevel) const
	{
		if (_xLevel.expired())
			return false;

		if (!m_SelectedEntity)
			return false;

		LevelBasePtr xLevel = _xLevel.lock();

		GUID* guidLevel = xLevel->GetEntityRegistry().try_ctx<Plop::GUID>();
		GUID* guidEntity = m_SelectedEntity.m_hEntity.registry().try_ctx<Plop::GUID>();
		if(guidLevel && guidEntity)
			return *guidLevel == *guidEntity;

		return false;
	}

	Entity EditorLayer::DuplicateEntity( const Entity& _entity )
	{
		LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
		const String& sName = _entity.GetComponent<Component_Name>().sName + " - copy";
		Entity dupEntity = xLevel->CreateEntity( sName );
		dupEntity.SetParent( _entity.GetParent() );

		entt::registry& reg = xLevel->m_ENTTRegistry;


		ComponentManager::DuplicateComponent( reg, _entity.m_hEntity.entity(), dupEntity.m_hEntity.entity() );

		_entity.VisitChildren( [&dupEntity](Entity _child ) {

			Entity dupChild = DuplicateEntity( _child );
			dupChild.SetParent(dupEntity);
			return VisitorFlow::CONTINUE;
		} );

		return dupEntity;
	}

	void EditorLayer::CopyEntity(const Entity& _entity)
	{
		json j = _entity.ToJson();
		j[JSON_CHILDREN].clear(); // do not include children
		j[JSON_COPY_TYPE] = Private::CopyType::ENTITY;
		ImGui::SetClipboardText(j.dump(2).c_str());
	}

	bool EditorLayer::CanPasteEntity(const Entity& _entityParent)
	{
		json j;
		try
		{
			const char *pClipboard = ImGui::GetClipboardText();
			if (pClipboard && *pClipboard)
			{
				j = json::parse(pClipboard);
				return j.contains(JSON_COPY_TYPE) && j[JSON_COPY_TYPE] == Private::CopyType::ENTITY;
			}
		}
		catch (json::parse_error&)
		{
		}

		return false;
	}

	Entity EditorLayer::PasteEntity(Entity& _entityParent)
	{
		json j = json::parse(ImGui::GetClipboardText());
		Entity newEntity = Application::GetCurrentLevel().lock()->CreateEntity(j[JSON_NAME]);
		newEntity.FromJson(j);
		newEntity.SetParent(_entityParent);
		return newEntity;
	}

	void EditorLayer::ToggleEntity(Entity& _entity)
	{
		bool bWasVisible = _entity.IsVisible();
		_entity.SetVisible(!bWasVisible);

		UndoManager::PushAction(UndoAction::EntityVisibility(_entity, bWasVisible, !bWasVisible));
	}

	//////////////////////////////////////////////////////////////////////////
	// EditorGizmo

	CameraWeakPtr		EditorGizmo::s_xCamera;
	glm::mat4			EditorGizmo::s_mViewProj = glm::identity<glm::mat4>();
	glm::vec2			EditorGizmo::s_vViewportPos = VEC2_0;
	glm::vec2			EditorGizmo::s_vViewportSize = VEC2_1;


	void EditorGizmo::Point( const glm::vec2& _vPoint, glm::vec3 _vColor /*= VEC3_1*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "Scene" ); // TODO: handle multiple scene viewport
		ImDrawList* drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect( window->Rect().Min, window->Rect().Max );

		glm::vec2 vSSPoint = GetSSPosition( glm::vec3( _vPoint, 0.f ) );

		drawList->AddCircle( vSSPoint, 6.f, ImColor( _vColor.x, _vColor.y, _vColor.z ) );

		if (window)
			drawList->PopClipRect();
	}

	void EditorGizmo::Circle(const glm::vec3 &_vPos, float _fRadius, glm::vec3 _vColor /*= COLOR_WHITE*/)
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "Scene" ); // TODO: handle multiple scene viewport
		ImDrawList* drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect( window->Rect().Min, window->Rect().Max );

		glm::vec2 vSSPoint = GetSSPosition(_vPos);
		float fRadiusPixel = GetSSDistance(_vPos, _fRadius);

		drawList->AddCircle( vSSPoint, fRadiusPixel, ImColor( _vColor.x, _vColor.y, _vColor.z ) );

		if (window)
			drawList->PopClipRect();
	}

	void EditorGizmo::Arc(const glm::vec3 &_vPos, float _fRadius, float _fStartAngle, float _fEndAngle, glm::vec3 _vColor /*= COLOR_WHITE*/)
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "Scene" ); // TODO: handle multiple scene viewport
		ImDrawList* drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect( window->Rect().Min, window->Rect().Max );

		glm::vec2 vSSPoint = GetSSPosition(_vPos);
		float fRadiusPixel = GetSSDistance(_vPos, _fRadius);

		drawList->PathArcTo(vSSPoint, fRadiusPixel, _fStartAngle, _fEndAngle);
		drawList->PathStroke(ImColor(_vColor.x, _vColor.y, _vColor.z), false);

		if (window)
			drawList->PopClipRect();
	}

	void EditorGizmo::FilledCircle(const glm::vec3 &_vPos, float _fRadius, glm::vec3 _vColor /*= COLOR_WHITE*/)
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "Scene" ); // TODO: handle multiple scene viewport
		ImDrawList* drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect( window->Rect().Min, window->Rect().Max );

		glm::vec2 vSSPoint = GetSSPosition(_vPos);
		float fRadiusPixel = GetSSDistance(_vPos, _fRadius);

		drawList->AddCircleFilled( vSSPoint, fRadiusPixel, ImColor( _vColor.x, _vColor.y, _vColor.z ) );

		if (window)
			drawList->PopClipRect();
	}

	void EditorGizmo::Line( const glm::vec3& _v1, const glm::vec3& _v2, glm::vec3 _vColor /*= COLOR_WHITE*/ )
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

	void EditorGizmo::AABB( const glm::vec3& _vMin, const glm::vec3& _vMax, glm::vec3 _vColor /*= COLOR_WHITE*/ )
	{
		ImGuiWindow* window = ImGui::FindWindowByName( "Scene" ); // TODO: handle multiple scene viewport
		ImDrawList* drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect( window->Rect().Min, window->Rect().Max );

		const glm::vec3 vMinMaxX = glm::vec3( _vMax.x, _vMin.y, _vMin.z );
		const glm::vec3 vMinMaxY = glm::vec3( _vMin.x, _vMax.y, _vMin.z );
		const glm::vec3 vMinMaxZ = glm::vec3( _vMin.x, _vMin.y, _vMax.z );
		const glm::vec3 vMaxMinX = glm::vec3( _vMin.x, _vMax.y, _vMax.z );
		const glm::vec3 vMaxMinY = glm::vec3( _vMax.x, _vMin.y, _vMax.z );
		const glm::vec3 vMaxMinZ = glm::vec3( _vMax.x, _vMax.y, _vMin.z );

		const glm::vec2 vSSMin = GetSSPosition( _vMin );
		const glm::vec2 vSSMax = GetSSPosition( _vMax );
		const glm::vec2 vSSMinMaxX = GetSSPosition( vMinMaxX );
		const glm::vec2 vSSMinMaxY = GetSSPosition( vMinMaxY );
		const glm::vec2 vSSMinMaxZ = GetSSPosition( vMinMaxZ );
		const glm::vec2 vSSMaxMinX = GetSSPosition( vMaxMinX );
		const glm::vec2 vSSMaxMinY = GetSSPosition( vMaxMinY );
		const glm::vec2 vSSMaxMinZ = GetSSPosition( vMaxMinZ );

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
	
	void EditorGizmo::Cube(const glm::vec3 &_vHalfSize, const glm::mat4 &_mTransform, glm::vec3 _vColor /*= COLOR_WHITE*/)
	{
		ImGuiWindow *window = ImGui::FindWindowByName("Scene"); // TODO: handle multiple scene viewport
		ImDrawList *drawList = window ? window->DrawList : ImGui::GetBackgroundDrawList();
		if (window)
			drawList->PushClipRect(window->Rect().Min, window->Rect().Max);

		const glm::vec3 vMinMaxX = _mTransform * glm::vec4(_vHalfSize.x, -_vHalfSize.y, -_vHalfSize.z, 1.f);
		const glm::vec3 vMinMaxY = _mTransform * glm::vec4(-_vHalfSize.x, _vHalfSize.y, -_vHalfSize.z, 1.f);
		const glm::vec3 vMinMaxZ = _mTransform * glm::vec4(-_vHalfSize.x, -_vHalfSize.y, _vHalfSize.z, 1.f);
		const glm::vec3 vMaxMinX = _mTransform * glm::vec4(-_vHalfSize.x, _vHalfSize.y, _vHalfSize.z, 1.f);
		const glm::vec3 vMaxMinY = _mTransform * glm::vec4(_vHalfSize.x, -_vHalfSize.y, _vHalfSize.z, 1.f);
		const glm::vec3 vMaxMinZ = _mTransform * glm::vec4(_vHalfSize.x, _vHalfSize.y, -_vHalfSize.z, 1.f);

		const glm::vec2 vSSMin = GetSSPosition(_mTransform * glm::vec4(-_vHalfSize, 1.f));
		const glm::vec2 vSSMax = GetSSPosition(_mTransform * glm::vec4(_vHalfSize, 1.f));
		const glm::vec2 vSSMinMaxX = GetSSPosition(vMinMaxX);
		const glm::vec2 vSSMinMaxY = GetSSPosition(vMinMaxY);
		const glm::vec2 vSSMinMaxZ = GetSSPosition(vMinMaxZ);
		const glm::vec2 vSSMaxMinX = GetSSPosition(vMaxMinX);
		const glm::vec2 vSSMaxMinY = GetSSPosition(vMaxMinY);
		const glm::vec2 vSSMaxMinZ = GetSSPosition(vMaxMinZ);

		const float fThickness = 2.f;
		ImColor col(_vColor.x, _vColor.y, _vColor.z);
		// Min X
		drawList->AddLine(vSSMin, vSSMinMaxY, col, fThickness);
		drawList->AddLine(vSSMin, vSSMinMaxZ, col, fThickness);
		drawList->AddLine(vSSMinMaxY, vSSMaxMinX, col, fThickness);
		drawList->AddLine(vSSMinMaxZ, vSSMaxMinX, col, fThickness);
		// Max X
		drawList->AddLine(vSSMinMaxX, vSSMaxMinY, col, fThickness);
		drawList->AddLine(vSSMinMaxX, vSSMaxMinZ, col, fThickness);
		drawList->AddLine(vSSMaxMinY, vSSMax, col, fThickness);
		drawList->AddLine(vSSMaxMinZ, vSSMax, col, fThickness);
		// Min Y
		drawList->AddLine(vSSMin, vSSMinMaxX, col, fThickness);
		drawList->AddLine(vSSMinMaxZ, vSSMaxMinY, col, fThickness);
		// Max Y
		drawList->AddLine(vSSMinMaxY, vSSMaxMinZ, col, fThickness);
		drawList->AddLine(vSSMaxMinX, vSSMax, col, fThickness);
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

		ImGui::Custom::PathCatmullCurve(drawList, vSSPoint1, vSSPoint2, vSSPoint3, vSSPoint4);
		
		if (window)
			drawList->PopClipRect();
	}


	void EditorGizmo::SetCamera( CameraWeakPtr _xCamera)
	{
		s_xCamera = _xCamera;
		if (!s_xCamera.expired())
		{
			const glm::mat4 &mView = s_xCamera.lock()->GetViewMatrix();
			const glm::mat4 &mProj = s_xCamera.lock()->GetProjectionMatrix();
			s_mViewProj = mProj * mView;
		}
		else
		{
			s_mViewProj = glm::identity<glm::mat4>();
		}
	}

	//void EditorGizmo::SetViewProjMatrix( const glm::mat4& _mView, const glm::mat4& _mProj )
	//{
	//	s_mViewProj = _mProj * _mView;
	//}

	void EditorGizmo::SetViewportPosAndSize( const glm::vec2& _vPos, const glm::vec2& _vSize )
	{
		s_vViewportPos = _vPos;
		s_vViewportSize = _vSize;
	}

	glm::vec2 EditorGizmo::GetSSPosition( const glm::vec3& _vPos )
	{
		ASSERT(!s_xCamera.expired());
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

	float EditorGizmo::GetSSDistance(const glm::vec3 &_vPos, float _fDist)
	{
		ASSERT(!s_xCamera.expired());

		auto xCamera = s_xCamera.lock();

		if (xCamera->IsPerspective())
		{
			// @check
			glm::vec3 vCamPos = xCamera->GetPosition();
			float fDist = glm::length(vCamPos - _vPos);
			float fFov = xCamera->GetPerspectiveFOV();
			float fSSDist = tan(fFov) * fDist;

			return fSSDist;
		}
		else
		{
			float fSize = xCamera->GetOrthographicSize();
			float fRatio = _fDist / fSize;
			float fDist = fRatio * s_vViewportSize.y;

			return fDist;
		}
	}
}
