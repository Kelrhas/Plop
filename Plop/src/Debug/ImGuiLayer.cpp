#include "Plop_pch.h"
#include "ImGuiLayer.h"

#include <imgui_impl_opengl3.h>
#include <imgui_impl_win32.h>

#include <Application.h>
#include <Imgui.h>
#include <ImGuizmo.h>

namespace Plop
{

	void ImGuiLayer::OnRegistered()
	{
		ImGui_ImplWin32_EnableDpiAwareness();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.Fonts->AddFontFromFileTTF( "Plop/assets/fonts/open_sans/OpenSans-Light.ttf", 18.f );
		io.Fonts->AddFontFromFileTTF( "Plop/assets/fonts/open_sans/OpenSans-Regular.ttf", 18.f );
		io.Fonts->AddFontFromFileTTF( "Plop/assets/fonts/open_sans/OpenSans-Italic.ttf", 18.f );
		io.Fonts->AddFontFromFileTTF( "Plop/assets/fonts/open_sans/OpenSans-Bold.ttf", 18.f );
		io.FontDefault = io.Fonts->Fonts[(int)ImGui::FontWeight::REGULAR];

		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init( Application::Get()->GetWindow().GetNativeWindow(), Application::Get()->GetWindow().GetRenderContext()->GetNativeContext ());
		ImGui_ImplOpenGL3_Init();
	}

	void ImGuiLayer::OnUnregistered()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnUpdate(TimeStep& _timeStep)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)Application::Get()->GetWindow().GetWidth(), (float)Application::Get()->GetWindow().GetHeight());
		io.DeltaTime = 1.f / 60.f;

	}

	void ImGuiLayer::NewFrame()
	{
		PROFILING_FUNCTION();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::EndFrame()
	{
		PROFILING_FUNCTION();
		ImGui::EndFrame();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			Application::Get()->GetWindow().GetRenderContext()->SetCurrent();
		}
	}
}

namespace ImGui
{
	void PushFontWeight( FontWeight eWeight )
	{
		PushFont( GetIO().Fonts->Fonts[(int)eWeight] );
	}

	void PopFontWeight()
	{
		PopFont();
	}
}