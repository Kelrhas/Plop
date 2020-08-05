#include "Plop_pch.h"
#include <Input/Input.h>

#include <Application.h>

#ifdef PLATFORM_WINDOWS
#include <Windowsx.h>

namespace Plop
{
	bool Input::Update(const TimeStep& _timeStep)
	{
		float fDeltaTime = _timeStep.GetSystemDeltaTime();

		for (int i = 0; i < MAX_INPUT_KEYS; ++i)
		{
			if (m_oKeyMap[i].uFlags & KEY_DOWN_FLAG)
			{
				m_oKeyMap[i].fTimeDown += fDeltaTime;
				m_oKeyMap[i].uFlags |= KEY_WASDOWN_FLAG;
			}
			else
			{
				m_oKeyMap[i].uFlags &= ~KEY_WASDOWN_FLAG;
			}
		}

		for (int i = 0; i < 5; ++i)
		{
			if (m_oMouseInfos[i].uFlags & KEY_DOWN_FLAG)
			{
				m_oMouseInfos[i].fTimeDown += fDeltaTime;
				m_oMouseInfos[i].uFlags |= KEY_WASDOWN_FLAG;
			}
			else
			{
				m_oMouseInfos[i].uFlags &= ~KEY_WASDOWN_FLAG;
			}
		}

		bool bResult = false;
		MSG message;
		while (PeekMessage(&message, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
		{
			switch (message.message)
			{
				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
				case WM_KEYDOWN:
				case WM_KEYUP:
				{
					uint32_t vkCode = (uint32_t)message.wParam;
					bool bWasDown = ((message.lParam & (1 << 30)) != 0);
					bool bIsDown = ((message.lParam & (1 << 31)) == 0);

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

					bResult = true;
				}
				break;

				case WM_CHAR:
				{
					char str[3] = "0\n";
					str[0] = (char)message.wParam;
					OutputDebugStringA(str);

					for (VoidFuncCharVec::const_iterator it(m_oEventChar.begin()); it != m_oEventChar.end(); ++it)
					{
						(*it)((char)message.wParam);
					}
					bResult = true;
				}
				break;

				default:
					break;
			}

			TranslateMessage(&message);
		}

		while (PeekMessage(&message, m_hWindow, WM_MOUSEFIRST, WM_MOUSELAST, PM_QS_INPUT | PM_REMOVE))
		{
			switch (message.message)
			{
				case WM_MOUSEMOVE:
				{
					glm::vec2 vMousePos(GET_X_LPARAM(message.lParam), GET_Y_LPARAM(message.lParam));
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
					bResult = true;
					break;
				}

				case WM_LBUTTONDOWN:
					if (!IsMouseLeftDown() && !IsMouseRightDown() && !IsMouseMiddleDown())
						::SetCapture(m_hWindow);
					m_oMouseInfos[0].uFlags |= KEY_DOWN_FLAG;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[0].begin()); it != m_oEventMouse[0].end(); ++it)
						(*it)(true);
					bResult = true;
					break;

				case WM_LBUTTONUP:
					m_oMouseInfos[0].uFlags &= ~KEY_DOWN_FLAG;
					m_oMouseInfos[0].fTimeDown = 0;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[0].begin()); it != m_oEventMouse[0].end(); ++it)
						(*it)(false);
					bResult = true;
					if (!IsMouseRightDown() && !IsMouseMiddleDown())
						::ReleaseCapture();
					break;

				case WM_RBUTTONDOWN:
					if (!IsMouseLeftDown() && !IsMouseRightDown() && !IsMouseMiddleDown())
						::SetCapture(m_hWindow);
					m_oMouseInfos[1].uFlags |= KEY_DOWN_FLAG;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[1].begin()); it != m_oEventMouse[1].end(); ++it)
						(*it)(true);
					bResult = true;
					break;

				case WM_RBUTTONUP:
					m_oMouseInfos[1].uFlags &= ~KEY_DOWN_FLAG;
					m_oMouseInfos[1].fTimeDown = 0;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[1].begin()); it != m_oEventMouse[1].end(); ++it)
						(*it)(false);
					bResult = true;
					if (!IsMouseLeftDown() && !IsMouseMiddleDown())
						::ReleaseCapture();
					break;

				case WM_MBUTTONDOWN:
					if (!IsMouseLeftDown() && !IsMouseRightDown() && !IsMouseMiddleDown())
						::SetCapture(m_hWindow);
					m_oMouseInfos[2].uFlags |= KEY_DOWN_FLAG;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[2].begin()); it != m_oEventMouse[2].end(); ++it)
						(*it)(true);
					bResult = true;
					break;

				case WM_MBUTTONUP:
					m_oMouseInfos[2].uFlags &= ~KEY_DOWN_FLAG;
					m_oMouseInfos[2].fTimeDown = 0;
					for (VoidFuncBoolVec::const_iterator it(m_oEventMouse[2].begin()); it != m_oEventMouse[2].end(); ++it)
						(*it)(false);
					bResult = true;
					if (!IsMouseLeftDown() && !IsMouseRightDown())
						::ReleaseCapture();
					break;

				case WM_MOUSEWHEEL:
					short iDelta = HIWORD(message.wParam);
					float fValue = ((float)iDelta) / WHEEL_DELTA;
					for (BoolFuncFloatVec::const_iterator it(m_oEventMouseWheel.begin()); it != m_oEventMouseWheel.end(); ++it)
					{
						if ((*it)(fValue))
							break;
					}
					bResult = true;
					break;

			}
		}

		return bResult;
	}
}

#endif
