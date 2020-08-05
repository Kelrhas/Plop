#include "Plop_pch.h"
#include "Input.h"

namespace Plop
{
	HWND										Input::m_hWindow;
	Input::KeyInfo								Input::m_oKeyMap[MAX_INPUT_KEYS];
	Input::KeyInfo								Input::m_oMouseInfos[5];
	std::vector<Input::VoidFuncKeyCodeBool>		Input::m_oEventKey;
	std::map<KeyCode, Input::VoidFuncBoolVec>	Input::m_oEventKeyMap;
	Input::VoidFuncCharVec						Input::m_oEventChar;
	Input::VoidFuncBoolVec						Input::m_oEventMouse[3];
	Input::BoolFuncFloatVec						Input::m_oEventMouseWheel;
	Input::MouseMoveCallbackVec					Input::m_oEventMouseMove;
	glm::vec2									Input::m_vMousePixelPos;
	glm::vec2									Input::m_vMouseViewportPos;

	void Input::Init(void* _pNativeWindow)
	{
		m_hWindow = (HWND)_pNativeWindow;
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
		return (m_oMouseInfos[0].uFlags & KEY_DOWN_FLAG) && !(m_oMouseInfos[0].uFlags & KEY_WASDOWN_FLAG);;
	}

	bool Input::IsMouseRightDown()
	{
		return m_oMouseInfos[1].uFlags & KEY_DOWN_FLAG;
	}

	bool Input::IsMouseMiddleDown()
	{
		return m_oMouseInfos[2].uFlags & KEY_DOWN_FLAG;
	}

	float Input::GetTimeDown(KeyCode eKeyCode)
	{
		return m_oKeyMap[(int)eKeyCode].fTimeDown;
	}

	char Input::GetCharFromCode(KeyCode eKeyCode)
	{
		return (char)eKeyCode;
	}

	glm::vec2 Input::GetCursorPixelPos()
	{
		return m_vMousePixelPos;
	}

	glm::vec2 Input::GetCursorViewportPos()
	{
		return m_vMouseViewportPos;
	}
}