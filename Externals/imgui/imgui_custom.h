
#include "imgui.h"

namespace ImGui
{

	ImGuiID		GetCurrentID();

	bool		DragBufferSize( const char* label, size_t* p_data, const size_t v_current, float v_speed, size_t v_max = 0, ImGuiSliderFlags flags = ImGuiSliderFlags_None );
	void		TextLabel( const char* label, const char* fmt, ... );
	void		TextLabelV( const char* label, const char* fmt, va_list args );

	// will show the path only between p2 and p3, p1 and p4 are there only for interpolation purpose
	void		PathCatmullCurve( ImDrawList* draw_list, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments = 12 );
		

	namespace Custom
	{
		void		Tooltip( const char* );
	}
}