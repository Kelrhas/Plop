#include "Plop_pch.h"
#include "OpenGL_Renderer.h"

#include <GL/glew.h>

namespace Plop
{

	void OpenGL_Renderer::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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



	int OpenGL_Renderer::GetMaxTextureUnit() const
	{
		int iMaxTexUnit = 32;
		//glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &iMaxTexUnit );
		// TODO: we set 32 in the shader for now
		return iMaxTexUnit;
	}
}