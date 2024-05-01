#include "Config.h"

#include "ImGuiLayer.h"

#include "Application.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"

#include <ImGuizmo.h>
#include <Imgui.h>

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
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.ConfigDockingTransparentPayload = true;

		io.Fonts->AddFontFromFileTTF((Application::Get()->GetEngineDirectory() / "assets/fonts/open_sans/OpenSans-Light.ttf").string().c_str(), 18.f);
		io.Fonts->AddFontFromFileTTF((Application::Get()->GetEngineDirectory() / "assets/fonts/open_sans/OpenSans-Regular.ttf").string().c_str(), 18.f);
		io.Fonts->AddFontFromFileTTF((Application::Get()->GetEngineDirectory() / "assets/fonts/open_sans/OpenSans-Italic.ttf").string().c_str(), 18.f);
		io.Fonts->AddFontFromFileTTF((Application::Get()->GetEngineDirectory() / "assets/fonts/open_sans/OpenSans-Bold.ttf").string().c_str(), 18.f);
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
		io.DeltaTime = _timeStep.GetSystemDeltaTime();
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