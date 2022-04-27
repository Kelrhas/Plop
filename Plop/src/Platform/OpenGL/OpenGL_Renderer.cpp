#include "Plop_pch.h"
#include "OpenGL_Renderer.h"

#include <GL/glew.h>

#include "Application.h"
#include "OpenGL_Debug.h"

namespace Plop
{

	void APIENTRY OpenGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
	{
		//if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
		//	return;

		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			return;

		String str("Opengl message: ");
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

		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:			str.append("\n\tSeverity: high");			Log::Error(str.c_str()); break;
			case GL_DEBUG_SEVERITY_MEDIUM:			str.append("\n\tSeverity: medium");			Log::Warn(str.c_str()); break;
			case GL_DEBUG_SEVERITY_LOW:				str.append("\n\tSeverity: low");			Log::Info(str.c_str()); break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:	str.append("\n\tSeverity: notification");	Log::Info(str.c_str()); break;
		}
	}


	void OpenGL_Renderer::Init()
	{
		GL_DEBUG_GROUP_SCOPED("Init");

		Log::Info(" GL Version {}", glGetString(GL_VERSION));
		Log::Info(" GLSL Version {}", glGetString(GL_SHADING_LANGUAGE_VERSION));
		Log::Info(" Renderer {}", glGetString(GL_RENDERER));

		glClearColor(0.f, 0.f, 0.f, 0.0f);

#ifndef _MASTER
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(OpenGLDebugMessageCallback, nullptr);
#endif

		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glEnable( GL_DEPTH_TEST );
		Debug::Assert_GL();
	}

	void OpenGL_Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Debug::Assert_GL();
	}

	void OpenGL_Renderer::Resize(uint32_t _uWidth, uint32_t _uHeight)
	{
		glViewport(0, 0, _uWidth, _uHeight);
		Debug::Assert_GL();
	}

	void OpenGL_Renderer::DrawIndexed(const VertexArrayPtr& _xVertexArray)
	{
		_xVertexArray->Draw();
		Debug::Assert_GL();
	}

	void OpenGL_Renderer::DrawFrameBuffer( const FrameBufferPtr& _xFramebuffer )
	{
		GL_DEBUG_GROUP_SCOPED( "DrawFrameBuffer" );

		glBindFramebuffer( GL_READ_FRAMEBUFFER, _xFramebuffer->GetID() );
		Debug::Assert_GL();

		glm::uvec2 vViewportSize = Application::Get()->GetWindow().GetViewportSize();

		glBlitFramebuffer(	0, 0, _xFramebuffer->GetWidth(), _xFramebuffer->GetHeight(),
							0, 0, vViewportSize.x, vViewportSize.y,
							GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT*/,
							GL_LINEAR);
		Debug::Assert_GL();
	}



	int OpenGL_Renderer::GetMaxTextureUnit() const
	{
		int iMaxTexUnit = 32;
		//glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &iMaxTexUnit );
		// TODO: we set 32 in the shader for now
		return iMaxTexUnit;
	}
}