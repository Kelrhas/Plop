#include "Plop_pch.h"
#include "Renderer.h"

#include <Platform/OpenGL/OpenGL_Renderer.h>

namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	// RenderAPI
	RenderAPI::API RenderAPI::s_eRenderer = RenderAPI::API::OPENGL;



	//////////////////////////////////////////////////////////////////////////
	// Renderer
	RenderAPI* Renderer::s_pAPI = new OpenGL_Renderer();
	const Camera* Renderer::s_pCurrentCamera = nullptr;

	void Renderer::PrepareScene(const Camera* _pCamera)
	{
		s_pCurrentCamera = _pCamera;
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Clear()
	{
		s_pAPI->Clear();
	}

	void Renderer::SubmitDraw(const ShaderPtr& _xShader, const VertexArrayPtr& _xVA)
	{
		_xShader->Bind();
		_xShader->SetUniformMat4("u_mVP", s_pCurrentCamera->GetViewProjectionMatrix());
		_xVA->Bind();
		s_pAPI->DrawIndexed(_xVA);
	}
}
