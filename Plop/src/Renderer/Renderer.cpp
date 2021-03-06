#include "Plop_pch.h"
#include "Renderer.h"

#include <Application.h>
#include <Platform/OpenGL/OpenGL_Renderer.h>
#include <ECS/BaseComponents.h>

namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	// RenderAPI
	RenderAPI::API RenderAPI::s_eRenderer = RenderAPI::API::OPENGL;



	//////////////////////////////////////////////////////////////////////////
	// Renderer

	void Renderer2D::Stats::Reset()
	{
		uDrawCalls = 0;
		uQuads = 0;
	}

	RenderAPI*			Renderer::s_pAPI = NEW OpenGL_Renderer();
	Renderer::SceneData Renderer::s_SceneData;
	ShaderLibrary		Renderer::s_shaderLibrary;
	FrameBufferPtr		Renderer::s_xFramebuffer = nullptr;

	void Renderer::Init()
	{
		s_pAPI->Init();

		glm::uvec2 vViewportSize = Application::Get()->GetWindow().GetViewportSize();
		if(!s_xFramebuffer)
			s_xFramebuffer = FrameBuffer::Create( vViewportSize.x, vViewportSize.y );
	}

	void Renderer::OnResize(uint32_t _uWidth, uint32_t _uHeight)
	{
		s_pAPI->Resize(_uWidth, _uHeight);
		if(s_xFramebuffer)
			s_xFramebuffer->Resize( _uWidth, _uHeight );
		else
			s_xFramebuffer = FrameBuffer::Create( _uWidth, _uHeight );
	}

	void Renderer::PrepareScene( const glm::mat4& _mProjectionMatrix, const glm::mat4& _mViewMatrix )
	{
		s_SceneData.mVPMatrix = _mProjectionMatrix * _mViewMatrix;
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
		if (_xMesh->m_hTex)
		{
			_xMesh->m_xShader->SetUniformInt( "u_tDiffuse", 0 );
			_xMesh->m_hTex->BindSlot( 0 );
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
	uint32_t						Renderer2D::MAX_TEX_UNIT = 32;
	bool							Renderer2D::s_bRendering2D = false;
	TexturePtr						Renderer2D::s_xWhiteTex = nullptr;
	TexturePtr						Renderer2D::s_xCheckerTex = nullptr;
	ShaderPtr						Renderer2D::s_xShader = nullptr;
	VertexArrayPtr					Renderer2D::s_xVertexArray = nullptr;
	VertexBufferPtr					Renderer2D::s_xVertexBuffer = nullptr;
	IndexBufferPtr					Renderer2D::s_xIndexBuffer = nullptr;
	Renderer2D::SceneData			Renderer2D::s_sceneData;

	const BufferLayout Renderer2D::Vertex::layout = {
			{ "in_position", BufferLayout::ElementType::FLOAT3},
			{ "in_color", BufferLayout::ElementType::FLOAT4},
			{ "in_uv", BufferLayout::ElementType::FLOAT2},
			{ "in_texUnit", BufferLayout::ElementType::FLOAT},
	};

	void Renderer2D::Init()
	{
		MAX_TEX_UNIT = Renderer::s_pAPI->GetMaxTextureUnit();
		s_sceneData.pTextureUnits = NEW const Texture*[MAX_TEX_UNIT];

		uint32_t uWhite = 0xFFFFFFFF;
		s_xWhiteTex = Texture::Create2D( 1, 1, (Texture::FlagsType)Texture::Flags::NONE, &uWhite, "white" );
		uint32_t checker[4] = { 0xAAAAAAFF, 0x333333FF, 0x333333FF, 0xAAAAAAFF };
		s_xCheckerTex = Texture::Create2D( 2, 2, (Texture::FlagsType)Texture::Flags::UV_REPEAT, &checker, "checker" );

		s_xShader = Plop::Renderer::LoadShader( "data/shaders/textured.glsl" );
		s_xShader->Bind();
		s_xShader->SetUniformVec4( "u_color", glm::vec4( 1.f ) );

		int* textures = NEW int[MAX_TEX_UNIT];
		for (uint32_t i = 0; i < MAX_TEX_UNIT; ++i)
			textures[i] = i;
		s_xShader->SetUniformIntArray( "u_textures", textures, MAX_TEX_UNIT );
		delete[] textures;


		s_xVertexArray = VertexArray::Create();
		s_xVertexArray->Bind();

		s_xVertexBuffer = VertexBuffer::Create( MAX_VERTICES * sizeof(Vertex) );
		s_xVertexBuffer->SetLayout( Vertex::layout );
		s_xVertexArray->AddVertexBuffer( s_xVertexBuffer );

		s_xIndexBuffer = IndexBuffer::Create( MAX_INDICES );
		s_xVertexArray->SetIndexBuffer( s_xIndexBuffer );

		s_sceneData.vecVertices.reserve( MAX_VERTICES );
		s_sceneData.vecIndices.reserve( MAX_INDICES );
	}

	void Renderer2D::NewFrame()
	{
		s_sceneData.frameStat.Reset();
		Renderer::s_xFramebuffer->Bind();
	}

	void Renderer2D::EndFrame()
	{
		Renderer::s_xFramebuffer->Unbind();

		Renderer::s_pAPI->DrawFrameBuffer( Renderer::s_xFramebuffer );
	}

	void Renderer2D::PrepareScene( const glm::mat4& _mProjectionMatrix, const glm::mat4& _mViewMatrix )
	{
		s_bRendering2D = true;
		s_xShader->Bind();
		glm::mat4 mProjView = _mProjectionMatrix * _mViewMatrix;
		s_xShader->SetUniformMat4( "u_mViewProjection", mProjView );
	}

	void Renderer2D::EndScene()
	{
		if (s_sceneData.uNbQuad > 0)
			DrawBatch();

		s_bRendering2D = false;
	}

	void Renderer2D::DrawQuad( const glm::vec4& _vColor, const glm::vec2& _vPos, const glm::vec2& _vSize )
	{
		glm::mat4 mTransform = glm::translate( glm::identity<glm::mat4>(), glm::vec3( _vPos, 0.f ) );
		mTransform = glm::scale( mTransform, glm::vec3( _vSize, 1.f ) );

		DrawQuad( _vColor, mTransform );
	}

	void Renderer2D::DrawQuad( const glm::vec4& _vColor, const glm::vec2& _vPos, const glm::vec2& _vSize, float _fAngleRad )
	{
		glm::mat4 mTransform = glm::translate( glm::identity<glm::mat4>(), glm::vec3( _vPos, 0.f ) );
		mTransform = glm::rotate( mTransform, _fAngleRad, glm::vec3( 0.f, 0.f, 1.f ) );
		mTransform = glm::scale( mTransform, glm::vec3( _vSize, 1.f ) );

		DrawQuad( _vColor, mTransform );
	}

	void Renderer2D::DrawQuad( const glm::vec4& _vColor, const glm::mat4& _mTransform )
	{
		PROFILING_FUNCTION();

		ASSERTM(s_bRendering2D, "Renderer2D::PrepareScene has not been called");


		if (s_sceneData.uNbQuad == MAX_QUADS || s_sceneData.uNbTex == MAX_TEX_UNIT)
			DrawBatch();

		Vertex v;
		v.vColor = _vColor;


		bool bTexFound = false;
		for (uint32_t i = 0; i < s_sceneData.uNbTex; ++i)
		{
			if (s_sceneData.pTextureUnits[i]->Compare( *s_xWhiteTex ))
			{
				v.fTexUnit = (float)i;
				bTexFound = true;
				break;
			}
		}
		if (!bTexFound)
		{
			s_sceneData.pTextureUnits[s_sceneData.uNbTex] = s_xWhiteTex.get();
			v.fTexUnit = (float)s_sceneData.uNbTex++;
		}

		v.vPosition = _mTransform * glm::vec4( -0.5f, -0.5f, 0.f, 1.f );
		v.vUV = glm::vec2( 0.f );
		s_sceneData.vecVertices.push_back( v );
		v.vPosition = _mTransform * glm::vec4( 0.5f, -0.5f, 0.f, 1.f );
		v.vUV.x = 1.f;
		s_sceneData.vecVertices.push_back( v );
		v.vPosition = _mTransform * glm::vec4( 0.5f, 0.5f, 0.f, 1.f );
		v.vUV.y = 1.f;
		s_sceneData.vecVertices.push_back( v );
		v.vPosition = _mTransform * glm::vec4( -0.5f, 0.5f, 0.f, 1.f );
		v.vUV.x = 0.f;
		s_sceneData.vecVertices.push_back( v );


		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 0 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 1 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 2 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 2 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 3 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 0 );

		++s_sceneData.uNbQuad;
	}

	void Renderer2D::DrawTexture( const Texture& _Texture, const glm::vec2& _vPos, const glm::vec2& _vSize, const glm::vec4& _vTint /*= glm::vec4( 1.f )*/ )
	{
		glm::mat4 mTransform = glm::translate( glm::identity<glm::mat4>(), glm::vec3( _vPos, 0.f ) );
		mTransform = glm::scale( mTransform, glm::vec3( _vSize, 1.f ) );

		DrawTexture( _Texture, mTransform, _vTint );
	}

	void Renderer2D::DrawTexture( const Texture& _Texture, const glm::vec2& _vPos, const glm::vec2& _vSize, float _fAngleRad, const glm::vec4& _vTint /*= glm::vec4( 1.f )*/ )
	{
		glm::mat4 mTransform = glm::translate( glm::identity<glm::mat4>(), glm::vec3( _vPos, 0.f ) );
		mTransform = glm::rotate( mTransform, _fAngleRad, glm::vec3( 0.f, 0.f, 1.f ) );
		mTransform = glm::scale( mTransform, glm::vec3( _vSize, 1.f ) );

		DrawTexture( _Texture, mTransform, _vTint );
	}

	void Renderer2D::DrawTexture( const Texture& _Texture, const glm::mat4& _mTransform, const glm::vec4& _vTint /*= glm::vec4( 1.f )*/ )
	{
		PROFILING_FUNCTION();

		ASSERTM( s_bRendering2D, "Renderer2D::PrepareScene has not been called" );


		if (s_sceneData.uNbQuad == MAX_QUADS || s_sceneData.uNbTex == MAX_TEX_UNIT)
			DrawBatch();

		Vertex v;
		v.vColor = _vTint;

		bool bTexFound = false;
		for (uint32_t i = 0; i < s_sceneData.uNbTex; ++i)
		{
			if (s_sceneData.pTextureUnits[i]->Compare( _Texture ))
			{
				v.fTexUnit = (float)i;
				bTexFound = true;
				break;
			}
		}
		if (!bTexFound)
		{
			s_sceneData.pTextureUnits[s_sceneData.uNbTex] = &_Texture;
			v.fTexUnit = (float)s_sceneData.uNbTex++;
		}

		v.vPosition = _mTransform * glm::vec4( -0.5f, -0.5f, 0.f, 1.f );
		v.vUV = glm::vec2( 0.f );
		s_sceneData.vecVertices.push_back( v );
		v.vPosition = _mTransform * glm::vec4( 0.5f, -0.5f, 0.f, 1.f );
		v.vUV.x = 1.f;
		s_sceneData.vecVertices.push_back( v );
		v.vPosition = _mTransform * glm::vec4( 0.5f, 0.5f, 0.f, 1.f );
		v.vUV.y = 1.f;
		s_sceneData.vecVertices.push_back( v );
		v.vPosition = _mTransform * glm::vec4( -0.5f, 0.5f, 0.f, 1.f );
		v.vUV.x = 0.f;
		s_sceneData.vecVertices.push_back( v );


		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 0 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 1 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 2 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 2 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 3 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 0 );

		++s_sceneData.uNbQuad;
	}

	void Renderer2D::DrawSprite( const Sprite& _sprite, const glm::vec2& _vPos, const glm::vec2& _vSize /*= glm::vec2( 1.f )*/, float _fAngleRad /*= 0.f*/ )
	{
		glm::mat4 mTransform = glm::translate( glm::identity<glm::mat4>(), glm::vec3( _vPos, 0.f ) );
		mTransform = glm::rotate( mTransform, _fAngleRad, glm::vec3( 0.f, 0.f, 1.f ) );
		mTransform = glm::scale( mTransform, glm::vec3( _vSize, 1.f ) );

		DrawSprite( _sprite, mTransform );
	}

	void Renderer2D::DrawSprite( const Sprite& _sprite, const glm::mat4& _mTransform )
	{
		PROFILING_FUNCTION();

		ASSERTM( s_bRendering2D, "Renderer2D::PrepareScene has not been called" );


		if (s_sceneData.uNbQuad == MAX_QUADS || s_sceneData.uNbTex == MAX_TEX_UNIT)
			DrawBatch();

		Vertex v;
		v.vColor = _sprite.GetTint();

		bool bTexFound = false;
		for (uint32_t i = 0; i < s_sceneData.uNbTex; ++i)
		{
			if (_sprite.GetTextureHandle())
			{
				if (s_sceneData.pTextureUnits[i]->Compare( _sprite.GetTextureHandle() ))
				{
					v.fTexUnit = (float)i;
					bTexFound = true;
					break;
				}
			}
			else // default to white texture
			{
				if (s_sceneData.pTextureUnits[i]->Compare( *s_xWhiteTex ))
				{
					v.fTexUnit = (float)i;
					bTexFound = true;
					break;
				}
			}
		}
		if (!bTexFound)
		{
			if (_sprite.GetTextureHandle())
			{
				s_sceneData.pTextureUnits[s_sceneData.uNbTex] = &(*_sprite.GetTextureHandle());
				v.fTexUnit = (float)s_sceneData.uNbTex++;
			}
			else // default to white texture
			{
				s_sceneData.pTextureUnits[s_sceneData.uNbTex] = s_xWhiteTex.get();
				v.fTexUnit = (float)s_sceneData.uNbTex++;
			}
		}

		v.vPosition = _mTransform * glm::vec4( -0.5f, -0.5f, 0.f, 1.f );
		v.vUV = _sprite.GetUVMin();
		s_sceneData.vecVertices.push_back( v );
		v.vPosition = _mTransform * glm::vec4( 0.5f, -0.5f, 0.f, 1.f );
		v.vUV.x = _sprite.GetUVMax().x;
		s_sceneData.vecVertices.push_back( v );
		v.vPosition = _mTransform * glm::vec4( 0.5f, 0.5f, 0.f, 1.f );
		v.vUV = _sprite.GetUVMax();
		s_sceneData.vecVertices.push_back( v );
		v.vPosition = _mTransform * glm::vec4( -0.5f, 0.5f, 0.f, 1.f );
		v.vUV.x = _sprite.GetUVMin().x;
		s_sceneData.vecVertices.push_back( v );


		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 0 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 1 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 2 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 2 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 3 );
		s_sceneData.vecIndices.push_back( s_sceneData.uNbQuad * 4 + 0 );

		++s_sceneData.uNbQuad;
	}

	void Renderer2D::DrawBatch()
	{
		PROFILING_FUNCTION();

		ASSERTM( s_bRendering2D, "Renderer2D::PrepareScene has not been called" );

		s_xVertexBuffer->SetData( s_sceneData.vecVertices.size() * sizeof(Vertex), (float*)s_sceneData.vecVertices.data() );
		s_xIndexBuffer->SetData( (uint32_t)s_sceneData.vecIndices.size(), s_sceneData.vecIndices.data() );

		for (uint32_t i = 0; i < s_sceneData.uNbTex; ++i)
		{
			s_sceneData.pTextureUnits[i]->BindSlot( i );
		}

		s_xVertexArray->Bind();
		Renderer::s_pAPI->DrawIndexed( s_xVertexArray );

		s_sceneData.frameStat.uDrawCalls++;
		s_sceneData.frameStat.uQuads += s_sceneData.uNbQuad;

		s_sceneData.uNbQuad = 0;
		s_sceneData.uNbTex = 0;
		s_sceneData.vecVertices.clear();
		s_sceneData.vecIndices.clear();
	}

}
