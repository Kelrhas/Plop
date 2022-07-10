#pragma once


namespace Plop
{
	enum class EditorStyle
	{
		ENTITY_VALID,
		ENTITY_HIDDEN,

		COUNT
	};

	using StyleData = std::vector<std::pair<ImGuiCol_, ImVec4>>;

	const StyleData g_style[(size_t)EditorStyle::COUNT]
	{
		// ENTITY_VALID
		{ },
		// ENTITY_HIDDEN
		{
			{ImGuiCol_Text, COLOR_GREY128},
			{ImGuiCol_Header, COLOR_GREY64},
			{ImGuiCol_HeaderHovered, COLOR_GREY64 * 1.2f},
			{ImGuiCol_HeaderActive, COLOR_GREY64 * 1.35f}
		}
	};
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