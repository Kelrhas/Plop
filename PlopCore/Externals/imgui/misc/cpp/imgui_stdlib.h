#pragma once

#include <string>
#include <filesystem>

namespace ImGui
{
	// ImGui::InputText() with std::string
	// Because text input needs dynamic resizing, we need to setup a callback to grow the capacity
	IMGUI_API bool  InputText(const char *label, std::string *str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void *user_data = NULL);
	IMGUI_API bool  InputTextMultiline(const char *label, std::string *str, const ImVec2 &size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void *user_data = NULL);
	IMGUI_API bool  InputTextWithHint(const char *label, const char *hint, std::string *str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void *user_data = NULL);


	// TODO : make it dynamically resizable, only read only for now
	IMGUI_API bool  InputText(const char *label, const std::filesystem::path &path, ImGuiInputTextFlags flags = ImGuiInputTextFlags_ReadOnly, ImGuiInputTextCallback callback = NULL, void *user_data = NULL);
}
