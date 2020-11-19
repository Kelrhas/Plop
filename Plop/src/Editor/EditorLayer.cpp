#include "Plop_pch.h"
#include "EditorLayer.h"

#include <imgui.h>
#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?
#include <imgui_entt_entity_editor.hpp>


#include <Application.h>
#include <Editor/Console.h>
#include <ECS/BaseComponents.h>
#include <ECS/LevelBase.h>
#include <Input/Input.h>
#include <Events/EventDispatcher.h>
#include <Events/EntityEvent.h>

namespace Plop
{
	::MM::EntityEditor<entt::entity> ENTTEditor;

#define REGISTER_COMPONENT(comp) ENTTEditor.registerComponent<comp##Component>( #comp )

	void EditorLayer::OnRegistered()
	{
		REGISTER_COMPONENT( Transform );
		REGISTER_COMPONENT( SpriteRenderer );
		REGISTER_COMPONENT( Camera );

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

		ImGui::PopStyleVar(3);

		static ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_PassthruCentralNode;
		static ImGuiID id = ImGui::GetID( "Editor" );
		ImGui::DockSpace( id, ImVec2(0, 0), flags );

		ShowMenuBar();


		// we can draw editor windows here

		if(m_bShowImGuiDemo)
			ImGui::ShowDemoWindow( &m_bShowImGuiDemo );

		if (m_bShowAllocations)
			Debug::ShowAllocationsWindow( &m_bShowAllocations );

		// TODO set docking to bottom
		Console::Draw();

		if (ImGui::Begin( "Scene graph" ))
		{
			if (!LevelBase::s_xCurrentLevel.expired())
			{
				static Entity entityToDestroy;
				static std::function<void( Entity& )> DrawEntity;
				DrawEntity = [this]( Entity& _Entity ) {

					ImGui::PushID( entt::to_integral( _Entity.m_EntityId ) );

					String& sName = _Entity.GetComponent<NameComponent>().sName;
					if (ImGui::Selectable( sName.c_str(), m_SelectedEntity == _Entity ))
					{
						m_SelectedEntity = _Entity;
					}

					if (ImGui::BeginPopupContextItem( "EntityContextMenu" ))
					{
						if (ImGui::Selectable( "New entity" ))
						{
							LevelBasePtr xLevel = LevelBase::s_xCurrentLevel.lock();
							Entity e = xLevel->CreateEntity();
							e.SetParent( _Entity );
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
				for( auto& e : view)
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

		if (m_SelectedEntity)
		{
			if (!LevelBase::s_xCurrentLevel.expired())
			{
				LevelBasePtr xLevel = LevelBase::s_xCurrentLevel.lock();
				ENTTEditor.render( xLevel->m_ENTTRegistry, m_SelectedEntity.m_EntityId );
			}
		}

		ImGui::End();
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


	void EditorLayer::ShowMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu( "File" ))
			{
				if (ImGui::MenuItem( "New level", nullptr, nullptr, false ))
				{
				}

				if (ImGui::MenuItem( "Open level", "Ctrl + O" ))
				{
					auto xLevel = Application::Get()->CreateNewLevel();
					m_SelectedEntity.Reset();
					xLevel->MakeCurrent();
					xLevel->Load();
				}

				if (ImGui::MenuItem( "Save level", "Ctrl + S", nullptr, !LevelBase::GetCurrentLevel().expired() ))
				{
					LevelBase::GetCurrentLevel().lock()->Save();
				}

				if (ImGui::MenuItem( "Close" ))
				{
					Application::Get()->Close();
				}

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

			ImGui::EndMenuBar();
		}

		// shortcuts for ImGui menus
		if (Input::IsKeyDown( KeyCode::KEY_Control ))
		{
			if (Input::IsKeyDown( KeyCode::KEY_O ))
			{
				auto xLevel = Application::Get()->CreateNewLevel();
				m_SelectedEntity.Reset();
				xLevel->MakeCurrent();
				xLevel->Load();
			}
			if (Input::IsKeyDown( KeyCode::KEY_S ) && !LevelBase::GetCurrentLevel().expired())
				LevelBase::GetCurrentLevel().lock()->Save();
		}
	}


	json EditorLayer::GetJsonEntity( const Entity& _entity )
	{
		json j;
		entt::entity entityID = _entity.m_EntityId;
		entt::registry& reg = _entity.m_xLevel.lock()->m_ENTTRegistry;

		for (auto& [component_type_id, ci] : ENTTEditor.component_infos)
		{
			if (ENTTEditor.entityHasComponent( reg, entityID, component_type_id ))
			{
				j[ci.name] = ci.tojson(reg, entityID);
			}
		}

		return j;
	}

	void EditorLayer::SetJsonEntity( const Entity& _entity, const json& _j )
	{
		entt::entity entityID = _entity.m_EntityId;
		entt::registry& reg = _entity.m_xLevel.lock()->m_ENTTRegistry;

		for (auto& [component_type_id, ci] : ENTTEditor.component_infos)
		{
			if (_j.contains( ci.name ))
			{
				ci.fromjson( reg, entityID, _j[ci.name] );
			}
		}
	}
}
