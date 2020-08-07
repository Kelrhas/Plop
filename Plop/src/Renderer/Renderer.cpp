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
	Renderer::SceneData Renderer::s_SceneData;
	ShaderLibrary Renderer::s_shaderLibrary;

	void Renderer::Init()
	{
		s_pAPI->Init();
	}

	void Renderer::OnResize(uint32_t _uWidth, uint32_t _uHeight)
	{
		s_pAPI->Resize(_uWidth, _uHeight);
	}

	void Renderer::PrepareScene(const Camera& _camera )
	{
		s_SceneData.mVPMatrix = _camera.GetViewProjectionMatrix();
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
		_xMesh->m_xShader->SetUniformMat4("u_mViewProjection", s_SceneData.mVPMatrix ); // will have to move into scene prepare command

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


	//////////////////////////////////////////////////////////////////////////
	// Renderer2D
	Mesh	Renderer2D::m_Quad;

	void Renderer2D::Init()
	{
		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
		};

		m_Quad.m_xVertexArray = VertexArray::Create();
		m_Quad.m_xVertexArray->Bind();

		BufferLayout layout = {
			{ "position", BufferLayout::ElementType::FLOAT3}
		};

		VertexBufferPtr xVertBuff = VertexBuffer::Create( (uint32_t)sizeof( vertices ) * 3, (float*)&vertices );
		xVertBuff->SetLayout( layout );
		m_Quad.m_xVertexArray->AddVertexBuffer( xVertBuff );


		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		IndexBufferPtr xIndBuff = IndexBuffer::Create( (uint32_t)sizeof( indices ) / sizeof( uint32_t ), indices );
		m_Quad.m_xVertexArray->SetIndexBuffer( xIndBuff );

		m_Quad.m_xShader = Plop::Renderer::LoadShader( "data/shaders/flatColor.glsl" );
	}

	void Renderer2D::PrepareScene( const OrthographicCamera& _camera )
	{
		m_Quad.m_xShader->Bind();
		m_Quad.m_xShader->SetUniformMat4( "u_mViewProjection", _camera.GetViewProjectionMatrix() );
	}

	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::Clear()
	{

	}

	void Renderer2D::DrawQuadColor( glm::vec2 _vPos, glm::vec2 _vSize, glm::vec4 _vColor )
	{
		DrawQuadColor( glm::vec3( _vPos, 0.f ), _vSize, _vColor );
	}

	void Renderer2D::DrawQuadColor( glm::vec3 _vPos, glm::vec2 _vSize, glm::vec4 _vColor )
	{
		m_Quad.m_mTransform = glm::translate( glm::identity<glm::mat4>(), _vPos );
		m_Quad.m_mTransform = glm::scale( m_Quad.m_mTransform, glm::vec3( _vSize, 1.f ) );


		m_Quad.m_xShader->SetUniformVec4( "u_color", _vColor );
		m_Quad.m_xShader->SetUniformMat4( "u_mModel", m_Quad.m_mTransform );

		m_Quad.m_xVertexArray->Bind();
		Renderer::s_pAPI->DrawIndexed( m_Quad.m_xVertexArray );
	}

}
