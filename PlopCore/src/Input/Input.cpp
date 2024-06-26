#include "Input.h"

namespace Plop
{
	HWND										Input::m_hWindow;
	Input::KeyInfo								Input::m_oKeyMap[MAX_INPUT_KEYS];
	Input::KeyInfo								Input::m_oMouseInfos[MAX_MOUSE_BUTTON];
	std::vector<Input::VoidFuncKeyCodeBool>		Input::m_oEventKey;
	std::map<KeyCode, Input::VoidFuncBoolVec>	Input::m_oEventKeyMap;
	Input::VoidFuncCharVec						Input::m_oEventChar;
	Input::VoidFuncBoolVec						Input::m_oEventMouse[MAX_MOUSE_BUTTON];
	Input::BoolFuncFloatVec						Input::m_oEventMouseWheel;
	Input::MouseMoveCallbackVec					Input::m_oEventMouseMove;
	glm::vec2									Input::m_vMousePixelPos;
	glm::vec2									Input::m_vMouseViewportPos;

	void Input::Init(void* _pNativeWindow)
	{
		m_hWindow = (HWND)_pNativeWindow;
	}

	void Input::Update(const TimeStep& _timeStep)
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

		for (int i = 0; i < MAX_MOUSE_BUTTON; ++i)
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
	}

	void Input::RegisterKey(KeyCode eKeycode, VoidFuncBool pEventFunc)
	{
		m_oEventKeyMap[eKeycode].push_back(pEventFunc);
	}

	void Input::RegisterKey(VoidFuncKeyCodeBool pEventFunc)
	{
		m_oEventKey.push_back(pEventFunc);
	}

	void Input::RegisterChar(VoidFuncChar pEventFunc)
	{
		m_oEventChar.push_back(pEventFunc);
	}

	void Input::RegisterLeftMouseButton(VoidFuncBool pEventFunc)
	{
		m_oEventMouse[0].push_back(pEventFunc);
	}

	void Input::RegisterRightMouseButton(VoidFuncBool pEventFunc)
	{
		m_oEventMouse[1].push_back(pEventFunc);
	}

	void Input::RegisterMiddleMouseButton(VoidFuncBool pEventFunc)
	{
		m_oEventMouse[2].push_back(pEventFunc);
	}

	void Input::RegisterMouseWheel(BoolFuncFloat pEventFunc)
	{
		m_oEventMouseWheel.push_back(pEventFunc);
	}

	void Input::RegisterMouseMove(MouseMoveCallback pEventFunc)
	{
		m_oEventMouseMove.push_back(pEventFunc);
	}

	bool Input::IsKeyPressed(KeyCode eKeyCode)
	{
		return (m_oKeyMap[(int)eKeyCode].uFlags & KEY_DOWN_FLAG) && !(m_oKeyMap[(int)eKeyCode].uFlags & KEY_WASDOWN_FLAG);
	}

	bool Input::IsKeyDown(KeyCode eKeyCode)
	{
		return m_oKeyMap[(int)eKeyCode].uFlags & KEY_DOWN_FLAG;
	}

	bool Input::IsMouseLeftDown()
	{
		return m_oMouseInfos[0].uFlags & KEY_DOWN_FLAG;
	}

	bool Input::IsMouseLeftPressed()
	{
		return (m_oMouseInfos[0].uFlags & KEY_DOWN_FLAG) && !(m_oMouseInfos[0].uFlags & KEY_WASDOWN_FLAG);
	}

	bool Input::IsMouseRightDown()
	{
		return m_oMouseInfos[1].uFlags & KEY_DOWN_FLAG;
	}

	bool Input::IsMouseRightPressed()
	{
		return (m_oMouseInfos[1].uFlags & KEY_DOWN_FLAG) && !(m_oMouseInfos[1].uFlags & KEY_WASDOWN_FLAG);
	}

	bool Input::IsMouseMiddleDown()
	{
		return m_oMouseInfos[2].uFlags & KEY_DOWN_FLAG;
	}

	bool Input::IsMouseMiddlePressed()
	{
		return (m_oMouseInfos[2].uFlags & KEY_DOWN_FLAG) && !(m_oMouseInfos[2].uFlags & KEY_WASDOWN_FLAG);
	}

	float Input::GetTimeDown(KeyCode eKeyCode)
	{
		return m_oKeyMap[(int)eKeyCode].fTimeDown;
	}

	char Input::GetCharFromCode(KeyCode eKeyCode)
	{
		return (char)eKeyCode;
	}

	const glm::vec2& Input::GetCursorPixelPos()
	{
		return m_vMousePixelPos;
	}

	const glm::vec2& Input::GetCursorWindowPos()
	{
		return m_vMouseViewportPos;
	}
}