#include "imgui.h"


namespace ImGui::Custom
{
	void Tooltip( const char* _pText )
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
			ImGui::TextUnformatted( _pText );
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}