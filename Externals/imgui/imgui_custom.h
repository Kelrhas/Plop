
#include "imgui.h"

namespace ImGui
{

	ImGuiID		GetCurrentID();

	bool		DragBufferSize( const char* label, size_t* p_data, const size_t v_current, float v_speed, size_t v_max = 0, ImGuiSliderFlags flags = ImGuiSliderFlags_None );
		

	namespace Custom
	{
		void		Tooltip( const char* );
	}
}