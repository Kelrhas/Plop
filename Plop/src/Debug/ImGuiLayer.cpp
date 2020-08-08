#include "Plop_pch.h"
#include "ImGuiLayer.h"

#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_win32.h>

#include <Application.h>
#include <Imgui.h>

namespace Plop
{

	void ImGuiLayer::OnRegistered()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(Application::Get()->GetWindow().GetNativeWindow());
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

		static bool b = true;
		if (b)
			ImGui::ShowDemoWindow(&b);
	}

	void ImGuiLayer::NewFrame()
	{
		PROFILING_FUNCTION();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::EndFrame()
	{
		PROFILING_FUNCTION();
		ImGui::EndFrame();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}