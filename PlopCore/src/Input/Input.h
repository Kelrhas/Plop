#pragma once

#include "Input/InputKeyCodes.h"
#include "TimeStep.h"

#include <functional>
#include <glm/vec2.hpp>
#include <map>
#include <vector>


#define KEY_DOWN_FLAG 0x0001
#define KEY_WASDOWN_FLAG 0x0002


namespace Plop
{
	class Input
	{
	public:
		struct KeyInfo
		{
			uint8_t uKeyCode;
			uint8_t uFlags;
			float	fTimeDown;

			KeyInfo() : uKeyCode(255), uFlags(0x0), fTimeDown(0.f) {}
		};
		using VoidFuncKeyCodeBool = std::function<void(KeyCode, bool)>;
		using MouseMoveCallback = std::function<void(glm::vec2, glm::vec2)>;
		using MouseMoveCallbackVec = std::vector<MouseMoveCallback>;
		using VoidFuncBool = std::function<void(bool)>;
		using VoidFuncChar = std::function<void(char)>;
		using BoolFuncFloat = std::function<bool(float)>;
		using VoidFuncBoolVec = std::vector<VoidFuncBool>;
		using VoidFuncCharVec = std::vector<VoidFuncChar>;
		using BoolFuncFloatVec = std::vector<BoolFuncFloat>;


		static void Init(void* _pNativeWindow);
		static void Update(const TimeStep& _timeStep);
#ifdef PLATFORM_WINDOWS
		static LRESULT Win32_Message(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

		static void RegisterKey(VoidFuncKeyCodeBool pEventFunc);
		static void RegisterKey(KeyCode eKeycode, VoidFuncBool pEventFunc);
		static void RegisterChar(VoidFuncChar pEventFunc);
		static void RegisterLeftMouseButton(VoidFuncBool pEventFunc);
		static void RegisterRightMouseButton(VoidFuncBool pEventFunc);
		static void RegisterMiddleMouseButton(VoidFuncBool pEventFunc);
		static void RegisterMouseWheel(BoolFuncFloat pEventFunc); // return whether to capture the input or to pass to other
		static void RegisterMouseMove(MouseMoveCallback pEventFunc);
		static bool IsKeyPressed(KeyCode eKeyCode); // pressed this frame
		static bool IsKeyDown(KeyCode eKeyCode); // held down, also true the first frame it is pressed
		static bool IsMouseLeftDown();
		static bool IsMouseLeftPressed();
		static bool IsMouseRightDown();
		static bool IsMouseRightPressed();
		static bool IsMouseMiddleDown();
		static bool IsMouseMiddlePressed();
		static float GetTimeDown(KeyCode eKeyCode);
		static char GetCharFromCode(KeyCode eKeyCode);

		static const glm::vec2& GetCursorPixelPos();
		static const glm::vec2& GetCursorWindowPos();


	protected:

		static HWND									m_hWindow;
		static KeyInfo								m_oKeyMap[MAX_INPUT_KEYS];
		static KeyInfo								m_oMouseInfos[MAX_MOUSE_BUTTON];
		static std::vector<VoidFuncKeyCodeBool>		m_oEventKey;
		static std::map<KeyCode, VoidFuncBoolVec>	m_oEventKeyMap;
		static VoidFuncCharVec						m_oEventChar;
		static VoidFuncBoolVec						m_oEventMouse[MAX_MOUSE_BUTTON];
		static BoolFuncFloatVec						m_oEventMouseWheel;
		static MouseMoveCallbackVec					m_oEventMouseMove;
		static glm::vec2							m_vMousePixelPos;
		static glm::vec2							m_vMouseViewportPos;

	};
}