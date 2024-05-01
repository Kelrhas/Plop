#include "Config.h"

#include "Platform/Win32/Win32_Window.h"

#include "Application.h"
#include "Constants.h"
#include "Debug/Log.h"
#include "Events/EventDispatcher.h"
#include "Events/WindowEvent.h"
#include "Input/Input.h"
#include "Platform/OpenGL/OpenGLWin32_Context.h"

#include <GL/glew.h>
#include <imgui.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Plop
{
	Window *Window::Create(WindowConfig &_config) { return NEW Win32_Window(_config); }


	Win32_Window::Win32_Window(WindowConfig &_config) : Window(_config), m_hWnd(NULL) {}

	Win32_Window::~Win32_Window() {}

	LRESULT Win32_Window::WindowCallback(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		LRESULT result = 0;


		result = ImGui_ImplWin32_WndProcHandler(_hWnd, _uMsg, _wParam, _lParam);
		if (result)
			return result;

		// 0 is captured
		if (Input::Win32_Message(_hWnd, _uMsg, _wParam, _lParam) == 0)
			return 0;

		switch (_uMsg)
		{
			case WM_CREATE:
			{
				m_hWnd = _hWnd;

				// prevent dpi scaling
				SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

				m_xRenderContext = RenderContext::Create();
				m_xRenderContext->Init();
			}
			break;

			case WM_DPICHANGED:
			{
				int		 iDpi	   = GetDpiForWindow(m_hWnd);
				ImGuiIO &io		   = ImGui::GetIO();
				io.FontGlobalScale = iDpi / 96.f;
			}
			break;


			case WM_MOVE:
			{
				m_config.uPosX = LOWORD(_lParam); // position of client area
				m_config.uPosY = HIWORD(_lParam);

				WindowMoveEvent event(*this);
				event.iNewPosX = m_config.uPosX;
				event.iNewPosY = m_config.uPosY;
				EventDispatcher::SendEvent(event);
			}
			break;

			case WM_SIZE:
			{
				WPARAM fwSizeType = _wParam; // resizing flag
				if (fwSizeType != SIZE_MINIMIZED)
				{
					m_config.uWidth	 = LOWORD(_lParam); // width of client area
					m_config.uHeight = HIWORD(_lParam); // height of client area

					WindowSizeEvent event(*this);
					event.iNewWidth	 = m_config.uWidth;	 // width of client area
					event.iNewHeight = m_config.uHeight; // height of client area
					EventDispatcher::SendEvent(event);
				}
				else
				{
					WindowSizeEvent event(*this);
					event.iNewWidth	 = 0;
					event.iNewHeight = 0;
					EventDispatcher::SendEvent(event);
				}
			}
			break;

			case WM_EXITSIZEMOVE:
			{
				Application::GetConfig().Save();
			}
			break;

			case WM_CLOSE:
				// if (MessageBox(m_hWnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
				{
					Application::Get()->Close();
				}
				break;


			case WM_DESTROY: PostQuitMessage(0); return 0;

			default: result = DefWindowProc(_hWnd, _uMsg, _wParam, _lParam); break;
		}

		return result;
	}

	static Win32_Window *s_pWaitingWindow = nullptr;
	LRESULT CALLBACK	 MainWindowCallback(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		if (_uMsg == WM_NCCREATE && s_pWaitingWindow != nullptr)
			SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(s_pWaitingWindow));
		else
		{
			Win32_Window *pThis = reinterpret_cast<Win32_Window *>(::GetWindowLongPtr(_hWnd, GWLP_USERDATA));
			if (pThis)
				return pThis->WindowCallback(_hWnd, _uMsg, _wParam, _lParam);
		}
		return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
	}

	void Win32_Window::Init()
	{
		HINSTANCE hInstance		  = NULL; // take the current
		WNDCLASS  windowClass	  = {};
		windowClass.style		  = CS_OWNDC; // | CS_HREDRAW | CS_VREDRAW;
		windowClass.hInstance	  = hInstance;
		windowClass.lpfnWndProc	  = MainWindowCallback;
		windowClass.lpszClassName = "MainWindow";
		windowClass.cbWndExtra	  = sizeof(Win32_Window *);

		DWORD windowStyle = WS_VISIBLE;
		if (m_config.bFullscreen)
		{
			windowStyle |= WS_POPUP | WS_MAXIMIZE;
		}
		else
		{
			windowStyle |= WS_OVERLAPPEDWINDOW | /*WS_SYSMENU |*/ WS_MINIMIZEBOX;
		}

		// retrieve the actual window size from a client size
		RECT windowSize = { (LONG)m_config.uPosX, (LONG)m_config.uPosY, (LONG)(m_config.uPosX + m_config.uWidth), (LONG)(m_config.uPosY + m_config.uHeight) };

		if (!AdjustWindowRectEx(&windowSize, windowStyle, false, 0))
		{
			return;
		}

		if (!RegisterClass(&windowClass))
			return;

		s_pWaitingWindow = this;
		m_sTitlePrefix	 = Application::Get()->GetName().c_str();
#if defined(_DEBUG)
		m_sTitlePrefix += " - Debug";
#elif defined(_RELEASE)
		m_sTitlePrefix += " - Release";
#elif defined(_MASTER)
		m_sTitlePrefix += " - Master";
#endif

		String sFullTitle = m_sTitlePrefix;
		if(!m_sTitleSuffix.empty())
			sFullTitle += " - " + m_sTitleSuffix;

		m_hWnd			 = CreateWindowEx(0,
								  "MainWindow",
								  sFullTitle.c_str(),
								  windowStyle,
								  windowSize.left,
								  windowSize.top,
								  windowSize.right - windowSize.left,
								  windowSize.bottom - windowSize.top,
								  nullptr,
								  nullptr,
								  hInstance,
								  this);
		s_pWaitingWindow = nullptr;
		ASSERT(m_hWnd);
	}

	void Win32_Window::Destroy()
	{
		m_xRenderContext->Destroy();
		DestroyWindow(m_hWnd);
	}

	void Win32_Window::Update(const TimeStep &_timeStep)
	{
		PROFILING_FUNCTION();

		// events
		MSG message;
		while (PeekMessage(&message, m_hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// definitely quit if needed
		while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				WindowCloseEvent event(*this);
				EventDispatcher::SendEvent(event);
			}
			else
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}
	}

	void Win32_Window::SwapBuffers() { m_xRenderContext->Flush(); }

	void Win32_Window::ToggleFullscreen()
	{
		static WINDOWPLACEMENT windowPlacement;
		DWORD				   windowStyle = GetWindowLong(m_hWnd, GWL_STYLE);
		if (windowStyle & WS_CAPTION)
		{
			MONITORINFO monitor = { sizeof(monitor) };
			if (GetWindowPlacement(m_hWnd, &windowPlacement) && GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &monitor))
			{
				SetWindowLong(m_hWnd, GWL_STYLE, windowStyle & ~WS_CAPTION);
				SetWindowPos(m_hWnd,
							 HWND_TOP,
							 monitor.rcMonitor.left,
							 monitor.rcMonitor.top,
							 monitor.rcMonitor.right - monitor.rcMonitor.left,
							 monitor.rcMonitor.bottom - monitor.rcMonitor.top,
							 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

				m_config.bFullscreen = true;
				Application::GetConfig().Save();
			}
		}
		else
		{
			SetWindowLong(m_hWnd, GWL_STYLE, windowStyle | WS_CAPTION);
			SetWindowPlacement(m_hWnd, &windowPlacement);
			SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			m_config.bFullscreen = false;
			Application::GetConfig().Save();
		}
	}

	void Win32_Window::SetVSync(bool _bEnabled) { m_xRenderContext->SetVSync(_bEnabled); }

	void Win32_Window::SetFullTitle() const
	{
		String sFullTitle = m_sTitlePrefix;
		if (!m_sTitleSuffix.empty())
			sFullTitle += " - " + m_sTitleSuffix;
		SetWindowTextA(m_hWnd, sFullTitle.c_str());
	}
} // namespace Plop