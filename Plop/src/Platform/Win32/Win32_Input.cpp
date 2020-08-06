#include "Plop_pch.h"
#include <Input/Input.h>

#include <imgui.h>
#include <Application.h>

#ifdef PLATFORM_WINDOWS
#include <Windowsx.h>

namespace Plop
{

	LRESULT Input::Win32_Message(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT result = -1;
		bool bHandleKeyboard = ImGui::GetCurrentContext() != nullptr ? !ImGui::GetIO().WantCaptureKeyboard : true;
		bool bHandleMouse = ImGui::GetCurrentContext() != nullptr ? !ImGui::GetIO().WantCaptureMouse : true;

		if(bHandleKeyboard)
		{
			switch (msg)
			{
				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
				case WM_KEYDOWN:
				case WM_KEYUP:
				{
					uint32_t vkCode = (uint32_t)wParam;
					bool bWasDown = ((lParam & (1 << 30)) != 0);
					bool bIsDown = ((lParam & (1 << 31)) == 0);

					KeyCode eKeyCode = (KeyCode)vkCode;
					if (bIsDown)
					{
						m_oKeyMap[(int)eKeyCode].uFlags |= KEY_DOWN_FLAG;
					}
					else
					{
						m_oKeyMap[(int)eKeyCode].uFlags &= ~KEY_DOWN_FLAG;
						m_oKeyMap[(int)eKeyCode].fTimeDown = 0;
					}

					if (bWasDown)
						m_oKeyMap[(int)eKeyCode].uFlags |= KEY_WASDOWN_FLAG;
					else
						m_oKeyMap[(int)eKeyCode].uFlags &= ~KEY_WASDOWN_FLAG;

					if (bIsDown != bWasDown)
					{
						for (VoidFuncBoolVec::const_iterator it(m_oEventKeyMap[eKeyCode].begin()); it != m_oEventKeyMap[eKeyCode].end(); ++it)
						{
							(*it)(bIsDown);
						}

						for (std::vector<VoidFuncKeyCodeBool>::const_iterator it(m_oEventKey.begin()); it != m_oEventKey.end(); ++it)
						{
							(*it)(eKeyCode, bIsDown);
						}
					}

					result = 0;
				}
				break;

				case WM_CHAR:
				{
					char str[3] = "0\n";
					str[0] = (char)wParam;
					OutputDebugStringA(str);

					for (VoidFuncCharVec::const_iterator it(m_oEventChar.begin()); it != m_oEventChar.end(); ++it)
					{
						(*it)((char)wParam);
					}
					result = 0;
				}
				break;

				default:
					break;
			}

		}

		if(bHandleMouse)
		{
			switch (msg)
			{
				case WM_MOUSEMOVE:
				{
					glm::vec2 vMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
					if (glm::any(glm::isnan(m_vMousePixelPos)))
						m_vMousePixelPos = vMousePos;
					else
					{
						glm::vec2 vViewportPos = vMousePos / Application::Get()->GetWindow().GetViewportSize();
						glm::vec2 vDelta = vViewportPos - m_vMouseViewportPos;

						for (MouseMoveCallbackVec::const_iterator it(m_oEventMouseMove.begin()); it != m_oEventMouseMove.end(); ++it)
							(*it)(vDelta, vViewportPos);
						m_vMousePixelPos = vMousePos;
						m_vMouseViewportPos = vViewportPos;
					}
					result = 0;
					break;
				}

				case WM_LBUTTONDOWN:
					if (!IsMouseLeftDown() && !IsMouseRightDown() && !IsMouseMiddleDown())
						::SetCapture(m_hWindow);
					m_oMouseInfos[0].uFlags |= KEY_DOWN_FLAG;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[0].begin()); it != m_oEventMouse[0].end(); ++it)
						(*it)(true);
					result = 0;
					break;

				case WM_LBUTTONUP:
					m_oMouseInfos[0].uFlags &= ~KEY_DOWN_FLAG;
					m_oMouseInfos[0].fTimeDown = 0;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[0].begin()); it != m_oEventMouse[0].end(); ++it)
						(*it)(false);
					result = 0;
					if (!IsMouseRightDown() && !IsMouseMiddleDown())
						::ReleaseCapture();
					break;

				case WM_RBUTTONDOWN:
					if (!IsMouseLeftDown() && !IsMouseRightDown() && !IsMouseMiddleDown())
						::SetCapture(m_hWindow);
					m_oMouseInfos[1].uFlags |= KEY_DOWN_FLAG;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[1].begin()); it != m_oEventMouse[1].end(); ++it)
						(*it)(true);
					result = 0;
					break;

				case WM_RBUTTONUP:
					m_oMouseInfos[1].uFlags &= ~KEY_DOWN_FLAG;
					m_oMouseInfos[1].fTimeDown = 0;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[1].begin()); it != m_oEventMouse[1].end(); ++it)
						(*it)(false);
					result = 0;
					if (!IsMouseLeftDown() && !IsMouseMiddleDown())
						::ReleaseCapture();
					break;

				case WM_MBUTTONDOWN:
					if (!IsMouseLeftDown() && !IsMouseRightDown() && !IsMouseMiddleDown())
						::SetCapture(m_hWindow);
					m_oMouseInfos[2].uFlags |= KEY_DOWN_FLAG;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[2].begin()); it != m_oEventMouse[2].end(); ++it)
						(*it)(true);
					result = 0;
					break;

				case WM_MBUTTONUP:
					m_oMouseInfos[2].uFlags &= ~KEY_DOWN_FLAG;
					m_oMouseInfos[2].fTimeDown = 0;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[2].begin()); it != m_oEventMouse[2].end(); ++it)
						(*it)(false);
					result = 0;
					if (!IsMouseLeftDown() && !IsMouseRightDown())
						::ReleaseCapture();
					break;

				case WM_MOUSEWHEEL:
					short iDelta = HIWORD(wParam);
					float fValue = ((float)iDelta) / WHEEL_DELTA;
					for (BoolFuncFloatVec::const_iterator it(m_oEventMouseWheel.begin()); it != m_oEventMouseWheel.end(); ++it)
					{
						if ((*it)(fValue))
							break;
					}
					result = 0;
					break;

			}
		}

		return result;
	}
}

#endif
