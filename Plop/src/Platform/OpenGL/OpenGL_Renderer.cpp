#include "Plop_pch.h"
#include "OpenGL_Renderer.h"

#include <GL/glew.h>

namespace Plop
{

	void OpenGL_Renderer::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGL_Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGL_Renderer::Resize(uint32_t _uWidth, uint32_t _uHeight)
	{
		glViewport(0, 0, _uWidth, _uHeight);
	}

	void OpenGL_Renderer::DrawIndexed(const VertexArrayPtr& _xVertexArray)
	{
		_xVertexArray->Draw();
	}
}