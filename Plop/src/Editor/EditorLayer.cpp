#include "Plop_pch.h"
#include "EditorLayer.h"

#include <imgui.h>

#include <Application.h>
#include <Editor/Console.h>

namespace Plop
{
	void EditorLayer::OnRegistered()
	{

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
