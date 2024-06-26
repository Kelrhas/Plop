#include "Config.h"

#include "imgui_custom.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
	#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui_internal.h>

namespace Private
{
	ImVec2 CatmullRom(const ImVec2 &_p0, const ImVec2 &_p1, const ImVec2 &_p2, const ImVec2 &_p3, float _t, float _alpha = 0.5f) // default to centripetal
	{
		_t	   = glm::clamp(_t, 0.f, 1.f);
		_alpha = glm::clamp(_alpha, 0.f, 1.f);


		auto GetT = [_alpha](float t, const ImVec2 &p0, const ImVec2 &p1) -> float {
			ImVec2 d = p1 - p0;
			float  a = d.x * d.x + d.y * d.y;
			float  b = glm::pow(a, 0.5f * _alpha);
			return b + t;
		};


		float t0 = 0;
		float t1 = GetT(t0, _p0, _p1);
		float t2 = GetT(t1, _p1, _p2);
		float t3 = GetT(t2, _p2, _p3);

		float t = (t2 - t1) * _t + t1;

		float t10 = t1 - t0;
		float t21 = t2 - t1;
		float t32 = t3 - t2;
		float t20 = t2 - t0;
		float t31 = t3 - t1;

		ImVec2 a1 = _p0 * ((t1 - t) / t10) + _p1 * ((t - t0) / t10);
		ImVec2 a2 = _p1 * ((t2 - t) / t21) + _p2 * ((t - t1) / t21);
		ImVec2 a3 = _p2 * ((t3 - t) / t32) + _p3 * ((t - t2) / t32);

		ImVec2 b1 = a1 * ((t2 - t) / t20) + a2 * ((t - t0) / t20);
		ImVec2 b2 = a2 * ((t3 - t) / t31) + a3 * ((t - t1) / t31);

		ImVec2 c = b1 * ((t2 - t) / t21) + b2 * ((t - t1) / t21);

		return c;
	}
} // namespace Private

namespace ImGui
{
	ImGuiID GetCurrentID()
	{
		ImGuiWindow *window = GImGui->CurrentWindow;
		return window->IDStack.back();
	}

	void TextLabel(const char *label, const char *fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		TextLabelV(label, fmt, args);
		va_end(args);
	}

	void TextLabelV(const char *label, const char *fmt, va_list args)
	{
		ImGuiWindow *window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext &	  g		= *GImGui;
		const ImGuiStyle &style = g.Style;
		const float		  w		= CalcItemWidth();

		const ImVec2 label_size		  = CalcTextSize(label, NULL, true);
		const char * value_text_begin = &g.TempBuffer[0];
		const char * value_text_end	  = value_text_begin + ImFormatStringV(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), fmt, args);
		const ImVec2 value_text_size  = CalcTextSize(value_text_begin, value_text_end);
		const ImRect value_bb(window->DC.CursorPos + ImVec2(w - value_text_size.x, 0.f),
							  window->DC.CursorPos + ImVec2(w, value_text_size.y + style.FramePadding.y * 2));
		const ImRect label_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2));
		const ImRect total_bb(
		  window->DC.CursorPos,
		  window->DC.CursorPos + ImVec2(w + (label_size.x > 0.0f ? style.ItemInnerSpacing.x : 0.0f), style.FramePadding.y * 2) + label_size);
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, 0))
			return;

		// Render
		RenderTextClipped(value_bb.Min, value_bb.Max, value_text_begin, value_text_end, NULL, ImVec2(0.0f, 0.f));
		if (label_size.x > 0.0f)
			RenderText(ImVec2(label_bb.Min.x, label_bb.Min.y + style.FramePadding.y), label);
	}

	bool ButtonRoundedEx(const char *label, const ImVec2 &size_arg, ImGuiButtonFlags flags, ImDrawCornerFlags draw_flags)
	{
		ImGuiWindow *window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext &	  g			 = *GImGui;
		const ImGuiStyle &style		 = g.Style;
		const ImGuiID	  id		 = window->GetID(label);
		const ImVec2	  label_size = CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		if ((flags & ImGuiButtonFlags_AlignTextBaseLine) &&
			style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline
																	  // matches (bit hacky, since it shouldn't be a flag)
			pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
		ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ItemSize(size, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;
		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

		// Render
		float		rounding = style.FrameRounding;
		const ImU32 col		 = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		RenderNavHighlight(bb, id);
		// RenderFrame( bb.Min, bb.Max, col, true, style.FrameRounding );
		{
			window->DrawList->AddRectFilled(bb.Min, bb.Max, col, rounding, draw_flags);
			const float border_size = g.Style.FrameBorderSize;
			if (border_size > 0.0f)
			{
				window->DrawList->AddRect(bb.Min + ImVec2(1, 1), bb.Max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow), rounding, draw_flags, border_size);
				window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(ImGuiCol_Border), rounding, draw_flags, border_size);
			}
		}
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

		// Automatically close popups
		// if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
		//    CloseCurrentPopup();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
		return pressed;
	}

	bool ButtonRounded(const char *label, const ImVec2 &size_arg, ImDrawCornerFlags draw_flags)
	{
		return ButtonRoundedEx(label, size_arg, ImGuiButtonFlags_None, draw_flags);
	}

	namespace Custom
	{
		bool DragBufferSize(const char *label, size_t *p_data, const size_t v_current, float v_speed, size_t v_max, ImGuiSliderFlags flags)
		{
			const char *const format	 = "%llu";
			const char *const apply_text = "Apply";
			const size_t	  v_min		 = 0;
			ImGuiDataType	  data_type	 = ImGuiDataType_U64;
			if constexpr (sizeof(size_t) == sizeof(uint32_t))
				data_type = ImGuiDataType_U32;

			ImGuiWindow *window = GetCurrentWindow();
			if (window->SkipItems)
				return false;

			ImGuiContext &	  g				= *GImGui;
			const ImGuiStyle &style			= g.Style;
			bool			  value_changed = false;
			BeginGroup();
			PushID(label);

			// Current value
			char current_value_text[256];
			sprintf_s(current_value_text, "%llu ->", v_current);
			const ImVec2 current_value_text_size = CalcTextSize(current_value_text, NULL, true);
			TextEx(current_value_text);
			SameLine();

			static const ImVec2 apply_text_size = CalcTextSize(apply_text, NULL, true);
			const float			button_width	= apply_text_size.x + style.ItemInnerSpacing.x + style.FramePadding.x * 2.f;
			PushItemWidth(CalcItemWidth() - button_width - style.ItemSpacing.x);
			size_t type_size = sizeof(size_t);

			// Drag
			value_changed |= DragScalar("", data_type, p_data, v_speed, &v_min, &v_max, format, flags);
			PopItemWidth();
			p_data = (size_t *)((char *)p_data + type_size);

			SameLine(0, g.Style.ItemInnerSpacing.x);

			// Button
			PushID(apply_text);
			const bool button_pushed = Button(apply_text);
			PopID();

			PopID();

			const char *label_end = FindRenderedTextEnd(label);
			if (label != label_end)
			{
				SameLine(0, g.Style.ItemInnerSpacing.x);
				TextEx(label, label_end);
			}

			EndGroup();
			return button_pushed;
		}

		void Tooltip(const char *_pText)
		{
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(_pText);
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}

		void PathCatmullCurve(ImDrawList *draw_list, const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, const ImVec2 &p4, int num_segments /*= 12*/)
		{
			draw_list->PathLineTo(p2);

			float t_step = 1.0f / (float)num_segments;
			for (int i_step = 1; i_step <= num_segments; i_step++)
				draw_list->_Path.push_back(Private::CatmullRom(p1, p2, p3, p4, t_step * i_step));

			draw_list->PathStroke(ImColor(1.f, 1.f, 1.f), false, 2.f);
		}

	} // namespace Custom
} // namespace ImGui