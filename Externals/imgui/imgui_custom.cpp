#include "imgui_custom.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"


namespace ImGui
{

	ImGuiID GetCurrentID()
	{
		ImGuiWindow* window = GImGui->CurrentWindow;
		return window->IDStack.back();
	}

	bool DragBufferSize( const char* label, size_t* p_data, const size_t v_current, float v_speed, size_t v_max, ImGuiSliderFlags flags )
	{
		const char* const format = "%llu";
		const char* const apply_text = "Apply";
		const size_t v_min = 0;
		ImGuiDataType data_type = ImGuiDataType_U64;
		if constexpr (sizeof( size_t ) == sizeof( uint32_t ))
			data_type = ImGuiDataType_U32;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		bool value_changed = false;
		BeginGroup();
		PushID( label );

		// Current value
		char current_value_text[256];
		sprintf_s( current_value_text, "%llu ->", v_current );
		const ImVec2 current_value_text_size = CalcTextSize( current_value_text, NULL, true );
		TextEx( current_value_text );
		SameLine();

		static const ImVec2 apply_text_size = CalcTextSize( apply_text, NULL, true );
		const float button_width = apply_text_size.x + style.ItemInnerSpacing.x + style.FramePadding.x * 2.f;
		PushItemWidth( CalcItemWidth() - button_width - style.ItemSpacing.x );
		size_t type_size = sizeof( size_t );

		// Drag
		value_changed |= DragScalar( "", data_type, p_data, v_speed, &v_min, &v_max, format, flags );
		PopItemWidth();
		p_data = (size_t*)((char*)p_data + type_size);

		SameLine( 0, g.Style.ItemInnerSpacing.x );

		// Button
		PushID( apply_text );
		const bool button_pushed = Button( apply_text );
		PopID();

		PopID();

		const char* label_end = FindRenderedTextEnd( label );
		if (label != label_end)
		{
			SameLine( 0, g.Style.ItemInnerSpacing.x );
			TextEx( label, label_end );
		}

		EndGroup();
		return button_pushed;
	}

	namespace Custom
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
}