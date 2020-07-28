#include <Plop_pch.h>

#include <GL/glew.h>
#include <GL/wglew.h>

#include <imgui.h>
#include <Platform/Win32/imgui_impl_win32.h>
#include <Platform/Win32/imgui_impl_opengl3.h>

#include <Platform/Win32/Win32_Window.h>
#include <Application.h>
#include <Log.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#include <Events/EventDispatcher.h>
#include <Events/WindowEvent.h>

namespace Plop
{

	Win_Window::Win_Window(WindowConfig& _config)
		: Window(_config)
		, m_hWnd(NULL)
		, m_hDeviceContext(NULL)
		, m_hGLRenderContext(NULL)
	{

	}

	Win_Window::~Win_Window()
	{

	}

	LRESULT Win_Window::WindowCallback(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		LRESULT result = 0;


		result = ImGui_ImplWin32_WndProcHandler(_hWnd, _uMsg, _wParam, _lParam);
		if (result)
			return true;

		switch (_uMsg)
		{
			case WM_CREATE:
			{
				m_hWnd = _hWnd;
				// prevent dpi scaling ?
				SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
				m_hDeviceContext = GetDC(m_hWnd);

				PIXELFORMATDESCRIPTOR pfd = {};
				pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
				pfd.nVersion = 1;
				pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER /*| PFD_GENERIC_ACCELERATED*/;
				pfd.iPixelType = PFD_TYPE_RGBA;				//The kind of framebuffer. RGBA or palette.
				pfd.cColorBits = 24;						//Colordepth of the framebuffer, without alpha
				pfd.cDepthBits = 24;						//Number of bits for the depthbuffer
				pfd.cStencilBits = 8;						//Number of bits for the stencil
				pfd.iLayerType = PFD_MAIN_PLANE;


				int  iWinPixelFormat = ChoosePixelFormat(m_hDeviceContext, &pfd);
				if (iWinPixelFormat == 0)
				{
					Log::Error("ChoosePixelFormat failed!");
					return -1;
				}

				//DescribePixelFormat(m_hDeviceContext, iWinPixelFormat, sizeof(pfd), &pfd);
				BOOL bResult = SetPixelFormat(m_hDeviceContext, iWinPixelFormat, &pfd);
				if (!bResult)
				{
					Log::Error("SetPixelFormat failed!");
					return -1;
				}


				HGLRC tempContext = wglCreateContext(m_hDeviceContext);
				wglMakeCurrent(m_hDeviceContext, tempContext);
				
				GLenum err = glewInit();
				if (err != GLEW_OK)
				{
					Log::Error("glewInit failed!");
					return -1;
				}

				if (wglewIsSupported("WGL_ARB_create_context") == 1)
				{
					int pixelFormatAttribs[] =
					{
						WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB,		GL_TRUE,
						WGL_DRAW_TO_WINDOW_ARB,					GL_TRUE,
						WGL_SUPPORT_OPENGL_ARB,					GL_TRUE,
						WGL_DOUBLE_BUFFER_ARB,					GL_TRUE,
						WGL_ACCELERATION_ARB,					WGL_FULL_ACCELERATION_ARB,
						WGL_PIXEL_TYPE_ARB,						WGL_TYPE_RGBA_ARB,
						WGL_COLOR_BITS_ARB,						32,
						WGL_DEPTH_BITS_ARB,						24,
						WGL_STENCIL_BITS_ARB,					8,
						0
					};

					//
					int iPixelFormat;
					UINT uNumFormats;
					wglChoosePixelFormatARB(m_hDeviceContext, pixelFormatAttribs, 0, 1, &iPixelFormat, &uNumFormats);
					if (!uNumFormats)
					{
						Log::Error("wglChoosePixelFormatARB failed!");
						return -1;
					}

					PIXELFORMATDESCRIPTOR pfd;
					DescribePixelFormat(m_hDeviceContext, iPixelFormat, sizeof(pfd), &pfd);
					if (!SetPixelFormat(m_hDeviceContext, iPixelFormat, &pfd))
					{
						Log::Error("SetPixelFormat failed!");
						return -1;
					}

					// version 4.4 minimum for glBufferStorage
					int contextAttribs[] =
					{
						WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
						WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
						WGL_CONTEXT_MINOR_VERSION_ARB, 4,
						WGL_CONTEXT_FLAGS_ARB, 0,
						0
					};

					m_hGLRenderContext = wglCreateContextAttribsARB(m_hDeviceContext, 0, contextAttribs);
					VERIFY(wglMakeCurrent(NULL, NULL));
					VERIFY(wglDeleteContext(tempContext));
					VERIFY(wglMakeCurrent(m_hDeviceContext, m_hGLRenderContext));
					//Assert_GL();
				}
				else
				{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
					m_hGLRenderContext = tempContext;
					return -1;
				}
			}
			break;

			case WM_DPICHANGED:
			{
				int iDpi = GetDpiForWindow(m_hWnd);
				ImGuiIO& io = ImGui::GetIO();
				io.FontGlobalScale = iDpi / 96.f;
			}
			break;


			case WM_MOVE:
			{
				m_config.uPosX = LOWORD(_lParam); // position of client area
				m_config.uPosY = HIWORD(_lParam);

				WindowMoveEvent event;
				event.iNewPosX = LOWORD( _lParam );
				event.iNewPosY = HIWORD( _lParam );
				EventDispatcher::SendEvent( event );
			}
			break;

			case WM_SIZE:
			{
				WPARAM fwSizeType = _wParam; // resizing flag
				/*if (g_pRenderer != nullptr && fwSizeType != SIZE_MINIMIZED)
				{*/
					m_config.uWidth = LOWORD(_lParam); // width of client area
					m_config.uHeight = HIWORD(_lParam); // height of client area 
					/*
					g_pRenderer->Resize(m_config.uWidth, m_config.uHeight);
				}*/

				WindowSizeEvent event;
				event.iNewWidth = LOWORD( _lParam ); // width of client area
				event.iNewHeight = HIWORD( _lParam ); // height of client area 
				EventDispatcher::SendEvent( event );
			}
			break;

			case WM_EXITSIZEMOVE:
			{
				Application::GetConfig().Save();
			}
			break;

			case WM_CLOSE:
				//if (MessageBox(m_hWnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
			{
				DestroyWindow(m_hWnd);
			}
			break;


			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;

			default:
				result = DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
				break;
		}

		return result;
	}

	void APIENTRY OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		//if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
		//	return;

		std::string str("Opengl message: ");
		str.append(message);

		switch (source)
		{
			case GL_DEBUG_SOURCE_API:				str.append("\n\tSource: API"); break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		str.append("\n\tSource: Window System"); break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER:	str.append("\n\tSource: Shader Compiler"); break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:		str.append("\n\tSource: Third Party"); break;
			case GL_DEBUG_SOURCE_APPLICATION:		str.append("\n\tSource: Application"); break;
			case GL_DEBUG_SOURCE_OTHER:				str.append("\n\tSource: Other"); break;
		}

		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:				str.append("\n\tType: Error"); break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	str.append("\n\tType: Deprecated Behaviour"); break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	str.append("\n\tType: Undefined Behaviour"); break;
			case GL_DEBUG_TYPE_PORTABILITY:			str.append("\n\tType: Portability"); break;
			case GL_DEBUG_TYPE_PERFORMANCE:			str.append("\n\tType: Performance"); break;
			case GL_DEBUG_TYPE_MARKER:				str.append("\n\tType: Marker"); break;
			case GL_DEBUG_TYPE_PUSH_GROUP:			str.append("\n\tType: Push Group"); break;
			case GL_DEBUG_TYPE_POP_GROUP:			str.append("\n\tType: Pop Group"); break;
			case GL_DEBUG_TYPE_OTHER:				str.append("\n\tType: Other"); break;
		}

		//ASSERT(severity == GL_DEBUG_SEVERITY_HIGH);
		/*
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:			str.append("\n\tSeverity: high");			Log::Error(str.c_str()); break;
			case GL_DEBUG_SEVERITY_MEDIUM:			str.append("\n\tSeverity: medium");			Log::Warn(str.c_str()); break;
			case GL_DEBUG_SEVERITY_LOW:				str.append("\n\tSeverity: low");			Log::Warn(str.c_str()); break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:	str.append("\n\tSeverity: notification");	Log::Info(str.c_str()); break;
		}
		*/
	}

	static Win_Window* s_pWaitingWindow = nullptr;
	LRESULT CALLBACK MainWindowCallback(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		if(_uMsg == WM_NCCREATE && s_pWaitingWindow != nullptr)
			SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(s_pWaitingWindow));
		else
		{
			Win_Window* pThis = reinterpret_cast<Win_Window*>(::GetWindowLongPtr(_hWnd, GWLP_USERDATA));
			if (pThis)
				return pThis->WindowCallback(_hWnd, _uMsg, _wParam, _lParam);
		}
		return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
	}

	void Win_Window::Init()
	{
		HINSTANCE hInstance = NULL; // take the current
		WNDCLASS windowClass = {};
		windowClass.style = CS_OWNDC;// | CS_HREDRAW | CS_VREDRAW;
		windowClass.hInstance = hInstance;
		windowClass.lpfnWndProc = MainWindowCallback;
		windowClass.lpszClassName = "MainWindow";
		windowClass.cbWndExtra = sizeof(Win_Window*);
		
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
		m_hWnd = CreateWindowEx(0, "MainWindow", "Test", windowStyle, windowSize.left, windowSize.top, windowSize.right - windowSize.left, windowSize.bottom - windowSize.top, nullptr, nullptr, hInstance, this);
		s_pWaitingWindow = nullptr;
		if (!m_hWnd)
			return;

		{
			ImGui::CreateContext();
			ImGui::StyleColorsDark();
			ImGui_ImplWin32_Init(m_hWnd);
			ImGui_ImplOpenGL3_Init();
		}

		Log::Info(" GL Version %s", glGetString(GL_VERSION));
		Log::Info(" GLSL Version %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		Log::Info(" Renderer %s", glGetString(GL_RENDERER));

		glClearColor(1.0f, 0.0f, 1.0f, 0.0f);

#ifndef _MASTER
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(OpenGLDebugMessageCallback, nullptr);
#endif

		SwapBuffers(m_hDeviceContext);
	}

	void Win_Window::Destroy()
	{
		
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		
		wglDeleteContext(m_hGLRenderContext);
	}

	void Win_Window::Update()
	{
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
				WindowCloseEvent event;
				EventDispatcher::SendEvent( event );
			}
			else
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(m_config.uWidth, m_config.uHeight);
		io.DeltaTime = 1.f / 60.f;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		static bool b = true;
		if(b)
			ImGui::ShowDemoWindow(&b);

		ImGui::EndFrame();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		// vsync if necessary
		glFlush();
		SwapBuffers(m_hDeviceContext);
	}

	void Win_Window::ToggleFullscreen()
	{
		static WINDOWPLACEMENT windowPlacement;
		DWORD windowStyle = GetWindowLong(m_hWnd, GWL_STYLE);
		if (windowStyle & WS_CAPTION)
		{
			MONITORINFO monitor = { sizeof(monitor) };
			if (GetWindowPlacement(m_hWnd, &windowPlacement) &&
				GetMonitorInfo(MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY), &monitor))
			{
				SetWindowLong(m_hWnd, GWL_STYLE, windowStyle & ~WS_CAPTION);
				SetWindowPos(m_hWnd, HWND_TOP,
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
			SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			m_config.bFullscreen = false;
			Application::GetConfig().Save();
		}

	}

	void Win_Window::SetVSync(bool _bEnabled)
	{
		if(_bEnabled)
			wglSwapIntervalEXT(1);
		else
			wglSwapIntervalEXT(0);
	}

	Window* Window::Create(WindowConfig& _config)
	{
		return new Win_Window(_config);
	}

}