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
	ShaderLibrary Renderer::s_shaderLibrary;

	void Renderer::Init()
	{
		s_pAPI->Init();
	}

	void Renderer::OnResize(uint32_t _uWidth, uint32_t _uHeight)
	{
		s_pAPI->Resize(_uWidth, _uHeight);
	}

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

	void Renderer::SubmitDraw(const MeshPtr& _xMesh)
	{
		_xMesh->m_xShader->Bind();
		_xMesh->m_xShader->SetUniformMat4("u_mViewProjection", s_pCurrentCamera->GetViewProjectionMatrix()); // will have to move into scene prepare command

		_xMesh->m_xShader->SetUniformMat4("u_mModel", _xMesh->m_mTransform);
		if (_xMesh->m_xTex)
		{
			_xMesh->m_xShader->SetUniformInt( "u_tDiffuse", 0 );
			_xMesh->m_xTex->BindSlot( 0 );
		}

		_xMesh->m_xVertexArray->Bind();
		s_pAPI->DrawIndexed(_xMesh->m_xVertexArray);
	}

	ShaderPtr Renderer::LoadShader( const String& _sFile )
	{
		return s_shaderLibrary.Load( _sFile );
	}
}
