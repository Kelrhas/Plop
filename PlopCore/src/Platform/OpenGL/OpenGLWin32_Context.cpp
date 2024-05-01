#include "Config.h"

#include "OpenGLWin32_Context.h"

#include "Application.h"
#include "Platform/Win32/Win32_Window.h"

// clang-format off
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/gl.h>
// clang-format on

namespace Plop
{

	void OpenGLWin32_Context::Init()
	{
		m_hDeviceContext = GetDC(((Win32_Window&)Application::Get()->GetWindow()).GetHWND());

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
			return;
		}

		//DescribePixelFormat(m_hDeviceContext, iWinPixelFormat, sizeof(pfd), &pfd);
		BOOL bResult = SetPixelFormat(m_hDeviceContext, iWinPixelFormat, &pfd);
		if (!bResult)
		{
			Log::Error("SetPixelFormat failed!");
			return;
		}


		HGLRC tempContext = wglCreateContext(m_hDeviceContext);
		wglMakeCurrent(m_hDeviceContext, tempContext);

		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			Log::Error("glewInit failed!");
			return;
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
				return;
			}

			PIXELFORMATDESCRIPTOR pfd;
			DescribePixelFormat(m_hDeviceContext, iPixelFormat, sizeof(pfd), &pfd);
			if (!SetPixelFormat(m_hDeviceContext, iPixelFormat, &pfd))
			{
				Log::Error("SetPixelFormat failed!");
				return;
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
			return;
		}
	}

	void OpenGLWin32_Context::Flush()
	{
		glFlush();
		SwapBuffers(m_hDeviceContext);
	}

	void OpenGLWin32_Context::Destroy()
	{
		wglDeleteContext(m_hGLRenderContext);
	}

	void OpenGLWin32_Context::SetCurrent()
	{
		VERIFY( wglMakeCurrent( m_hDeviceContext, m_hGLRenderContext ) );
	}

	void OpenGLWin32_Context::SetVSync(bool _bEnabled)
	{
		if (_bEnabled)
			wglSwapIntervalEXT(1);
		else
			wglSwapIntervalEXT(0);
	}
}