#include "Plop_pch.h"
#include "EditorLayer.h"

#include <imgui.h>
#include <imgui_entt_entity_editor.hpp>


#include <Application.h>
#include <Editor/Console.h>
#include <ECS/BaseComponents.h>
#include <ECS/Level.h>

namespace Plop
{
	::MM::EntityEditor<entt::entity> ENTTEditor;

#define REGISTER_COMPONENT(comp) ENTTEditor.registerComponent<comp##Component>( #comp )

	void EditorLayer::OnRegistered()
	{
		REGISTER_COMPONENT( Transform );
		REGISTER_COMPONENT( SpriteRenderer );
		REGISTER_COMPONENT( Camera );
	}

	void EditorLayer::OnUnregistered()
	{

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

		// TODO set docking to bottom
		Console::Draw();

		if (ImGui::Begin( "Scene graph" ))
		{
			if (!Level::s_xCurrentLevel.expired())
			{
				LevelPtr xLevel = Level::s_xCurrentLevel.lock();

				auto& view = xLevel->m_ENTTRegistry.view<NameComponent>();
				view.each( [this](entt::entity _e, const NameComponent& _comp) {
					Entity e{ _e, Level::s_xCurrentLevel };

					if (ImGui::Selectable( _comp.sName.c_str(), m_SelectedEntity == e ))
					{
						m_SelectedEntity = e;
					}
				} );
			}
		}
		ImGui::End();

		if (m_SelectedEntity)
		{
			if (!Level::s_xCurrentLevel.expired())
			{
				LevelPtr xLevel = Level::s_xCurrentLevel.lock();
				ENTTEditor.render( xLevel->m_ENTTRegistry, m_SelectedEntity.m_EntityId );
			}
		}

		ImGui::End();
	}

	bool EditorLayer::OnEvent( Event& _event )
	{
		return false;
	}


	void EditorLayer::ShowMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu( "File" ))
			{
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

			ImGui::EndMenuBar();
		}
	}
}
