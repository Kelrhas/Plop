#pragma once


namespace Plop
{
	enum class EditorStyle
	{
		DISABLED,
		ENTITY_VALID,
		ENTITY_HIDDEN,
		PREFAB,
		PREFAB_INVALID,

		COUNT
	};

	using StyleData = std::vector<std::pair<ImGuiCol_, ImVec4>>;

	const StyleData g_style[] {
		// DISABLED
		{
		  { ImGuiCol_Text, COLOR_GREY128 },
		},
		// ENTITY_VALID
		{},
		// ENTITY_HIDDEN
		{ { ImGuiCol_Text, COLOR_GREY128 },
		  { ImGuiCol_Header, ColorFromU32RGB(0x404040, 255) },
		  { ImGuiCol_HeaderHovered, ColorFromU32RGB(0x4D4D4D, 255) },
		  { ImGuiCol_HeaderActive, ColorFromU32RGB(0x565656, 255) } },
		// PREFAB
		{
		  { ImGuiCol_Text, ColorFromU32RGB(0x0196F9, 255) },
		  { ImGuiCol_HeaderHovered, ColorFromU32RGB(0x2A5E99, 255) },
		  { ImGuiCol_HeaderActive, ColorFromU32RGB(0x3A6EA9, 255) },
		},
		// PREFAB_INVALID
		{ { ImGuiCol_Text, ColorFromU32RGB(0xC72E2E, 255) } },
	};
	static_assert(sizeof(g_style) / sizeof(StyleData) == (size_t)EditorStyle::COUNT, "Missing StyleData");
}

namespace ImGui
{
	void PushStyleColor(Plop::EditorStyle _eStyle)
	{
		const auto& vecData = Plop::g_style[(size_t)_eStyle];
		for (const auto& [e, col] : vecData)
		{
			ImGui::PushStyleColor(e, col);
		}
	}

	void PopStyleColor(Plop::EditorStyle _eStyle)
	{
		ImGui::PopStyleColor(Plop::g_style[(size_t)_eStyle].size());
	}
}