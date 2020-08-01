#include "Plop_pch.h"
#include "OpenGL_Renderer.h"

#include <GL/glew.h>

namespace Plop
{

	void OpenGL_Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGL_Renderer::DrawIndexed(const VertexArrayPtr& _xVertexArray)
	{
		_xVertexArray->Draw();
	}
}