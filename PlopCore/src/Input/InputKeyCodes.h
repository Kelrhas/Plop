#pragma once

#include "Types.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else 
#error TODO
#endif

// for binded action, see https://github.com/depp/keycode/
// we really need to use ScanCode (physical place on the keyboard) instead of virtual keys (mapped keys depending on layout)

namespace Plop
{
	constexpr U32 MAX_INPUT_KEYS = 256;
	constexpr U8 MAX_MOUSE_BUTTON = 5;

	enum class KeyCode : U32
	{
		KEY_Backspace = VK_BACK,
		KEY_Tab = VK_TAB,
		KEY_Enter = VK_RETURN,
		KEY_Shift = VK_SHIFT,
		KEY_Control = VK_CONTROL,
		KEY_Alt = VK_MENU,
		KEY_Pause = VK_PAUSE,
		KEY_CAPSLOCK = VK_CAPITAL,
		KEY_Escape = VK_ESCAPE,
		KEY_Space = VK_SPACE,
		KEY_PageUp = VK_PRIOR,
		KEY_PageDown = VK_NEXT,
		KEY_End = VK_END,
		KEY_Home = VK_HOME,
		KEY_Right = VK_RIGHT,
		KEY_Left = VK_LEFT,
		KEY_Down = VK_DOWN,
		KEY_Up = VK_UP,
		KEY_Insert = VK_INSERT,
		KEY_Delete = VK_DELETE,
		KEY_0 = '0',
		KEY_1,
		KEY_2,
		KEY_3,
		KEY_4,
		KEY_5,
		KEY_6,
		KEY_7,
		KEY_8,
		KEY_9,
		KEY_A = 'A',
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z,
		//VK_LWIN 	91 	Left Windows key ( Microsoft® Natural® keyboard )
		//VK_RWIN 	92 	Right Windows key ( Natural keyboard )
		KEY_Context = VK_APPS,
		KEY_Pad0 = VK_NUMPAD0,
		KEY_Pad1 = VK_NUMPAD1,
		KEY_Pad2 = VK_NUMPAD2,
		KEY_Pad3 = VK_NUMPAD3,
		KEY_Pad4 = VK_NUMPAD4,
		KEY_Pad5 = VK_NUMPAD5,
		KEY_Pad6 = VK_NUMPAD6,
		KEY_Pad7 = VK_NUMPAD7,
		KEY_Pad8 = VK_NUMPAD8,
		KEY_Pad9 = VK_NUMPAD9,
		KEY_PadMultiply = VK_MULTIPLY,
		KEY_PadPlus = VK_ADD,
		KEY_PadMinus = VK_SUBTRACT,
		KEY_PadDecimal = VK_DECIMAL,
		KEY_PadDivide = VK_DIVIDE,
		KEY_F1 = VK_F1,
		KEY_F2 = VK_F2,
		KEY_F3 = VK_F3,
		KEY_F4 = VK_F4,
		KEY_F5 = VK_F5,
		KEY_F6 = VK_F6,
		KEY_F7 = VK_F7,
		KEY_F8 = VK_F8,
		KEY_F9 = VK_F9,
		KEY_F10 = VK_F10,
		KEY_F11 = VK_F11,
		KEY_F12 = VK_F12,
		KEY_F13 = VK_F13,
		KEY_F14 = VK_F14,
		KEY_F15 = VK_F15,
		KEY_F16 = VK_F16,
		KEY_F17 = VK_F17,
		KEY_F18 = VK_F18,
		KEY_F19 = VK_F19,
		KEY_F20 = VK_F20,
		KEY_F21 = VK_F21,
		KEY_F22 = VK_F22,
		KEY_F23 = VK_F23,
		KEY_F24 = VK_F24,
		KP_NumLock = VK_NUMLOCK,
		KEY_ScrollLock = VK_SCROLL,
		KEY_LeftControl = VK_LCONTROL,
		KEY_LeftShift = VK_LSHIFT,
		KEY_LeftAlt = VK_LMENU,
		KEY_RightControl = VK_RCONTROL,
		KEY_RightShift = VK_RSHIFT,
		KEY_RightAlt = VK_RMENU,
		KEY_Comma = VK_OEM_COMMA,
		// [...]
		KEY_OEM7 = VK_OEM_7, // ²
	};
}