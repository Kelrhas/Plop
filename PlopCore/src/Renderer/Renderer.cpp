#include "Config.h"

#include "Renderer.h"

#include "Application.h"
#include "ECS/Components/BaseComponents.h"
#include "Platform/OpenGL/OpenGL_Renderer.h"


namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	// RenderAPI
	//////////////////////////////////////////////////////////////////////////
	RenderAPI::API RenderAPI::s_eRenderer = RenderAPI::API::OPENGL;


	//////////////////////////////////////////////////////////////////////////
	// Renderer
	//////////////////////////////////////////////////////////////////////////

	namespace // Local storage
	{
		const uint32_t MAX_TEX_UNIT = 32; // TODO get the info from the API
		const uint32_t MAX_QUADS	= 10000;
		const uint32_t MAX_VERTICES = MAX_QUADS * 4;
		const uint32_t MAX_INDICES	= MAX_QUADS * 6;

		RenderAPIPtr		s_pAPI = nullptr;
		Renderer::SceneData s_sceneData;
		ShaderLibrary		s_shaderLibrary;
		FrameBufferPtr		s_xFramebuffer = nullptr;

		TexturePtr		s_defaultTextures[(int)Renderer::DefaultTexture::COUNT];
		ShaderPtr		s_xShader;
		VertexArrayPtr	s_xVertexArray;
		VertexBufferPtr s_xVertexBuffer;
		IndexBufferPtr	s_xIndexBuffer;
	} // namespace

	namespace Renderer
	{
		// ------------------------------------------------
		// Forward declaration of local functions
		void DrawBatch();


		// ------------------------------------------------
		// Renderer::Stats

		void Stats::Reset()
		{
			uDrawCalls = 0;
			uQuads	   = 0;
		}

		// ------------------------------------------------
		// Renderer::Vertex

		const BufferLayout Vertex::layout = {
			{ "in_position", BufferLayout::ElementType::FLOAT3 }, { "in_color", BufferLayout::ElementType::FLOAT4 },
			{ "in_uv", BufferLayout::ElementType::FLOAT2 },		  { "in_texUnit", BufferLayout::ElementType::FLOAT },
			{ "in_entityID", BufferLayout::ElementType::FLOAT },
		};

		// ------------------------------------------------
		// Function definitions

		void Init()
		{
			s_pAPI = Create();
			s_pAPI->Init();

			glm::uvec2 vViewportSize = Application::Get()->GetWindow().GetViewportSize();
			if (!s_xFramebuffer)
			{
				FrameBuffer::Specification specs;
				specs.uWidth			   = vViewportSize.x;
				specs.uHeight			   = vViewportSize.y;
				specs.vecRenderTargetSpecs = { FrameBuffer::TextureFormat::RGBA8, FrameBuffer::TextureFormat::UINT32, FrameBuffer::TextureFormat::DEPTH24S8 };
				s_xFramebuffer			   = FrameBuffer::Create(specs);
			}


			ASSERTM(MAX_TEX_UNIT >= s_pAPI->GetMaxTextureUnit(), "We have %d texture unit available, we need 32", s_pAPI->GetMaxTextureUnit());

			s_sceneData.pTextureUnits = NEW const Texture *[MAX_TEX_UNIT];

			uint32_t uWhite									= 0xFFFFFFFF;
			s_defaultTextures[(int)DefaultTexture::WHITE]	= Texture::Create2D(1, 1, (Texture::FlagsType)Texture::Flags::NONE, &uWhite, "white");
			uint32_t checker[4]								= { 0xAAAAAAFF, 0x333333FF, 0x333333FF, 0xAAAAAAFF };
			s_defaultTextures[(int)DefaultTexture::CHECKER] = Texture::Create2D(2, 2, (Texture::FlagsType)Texture::Flags::UV_REPEAT, &checker, "checker");

			s_xShader = LoadShader(Application::Get()->GetEngineDirectory() / "data/shaders/textured.glsl");
			s_xShader->Bind();
			s_xShader->SetUniformVec4("u_color", glm::vec4(1.f));

			// init the indices for the texture units
			int *textures = NEW int[MAX_TEX_UNIT];
			for (uint32_t i = 0; i < MAX_TEX_UNIT; ++i)
				textures[i] = i;
			s_xShader->SetUniformIntArray("u_textures", textures, MAX_TEX_UNIT);
			delete[] textures;

			s_xVertexArray = VertexArray::Create();
			s_xVertexArray->Bind();

			s_xVertexBuffer = VertexBuffer::Create(MAX_VERTICES * sizeof(Vertex));
			s_xVertexBuffer->SetLayout(Vertex::layout);
			s_xVertexArray->AddVertexBuffer(s_xVertexBuffer);

			s_xIndexBuffer = IndexBuffer::Create(MAX_INDICES);
			s_xVertexArray->SetIndexBuffer(s_xIndexBuffer);

			s_sceneData.vecVertices.reserve(MAX_VERTICES);
			s_sceneData.vecIndices.reserve(MAX_INDICES);
		}

		void OnResize(uint32_t _uWidth, uint32_t _uHeight)
		{
			if (!s_pAPI)
				return;
			s_pAPI->Resize(_uWidth, _uHeight);
			if (s_xFramebuffer)
				s_xFramebuffer->Resize(_uWidth, _uHeight);
		}

		void Renderer::NewFrame()
		{
			s_sceneData.frameStat.Reset();
			Clear();
		}

		void Renderer::EndFrame()
		{
			s_xFramebuffer->Unbind();

			if constexpr (USE_EDITOR)
			{
				if (!Application::Get()->IsInEditor())
					s_pAPI->DrawFrameBuffer(s_xFramebuffer);
			}
		}

		void PrepareScene(const glm::mat4 &_mProjectionMatrix, const glm::mat4 &_mViewMatrix)
		{
			ASSERTM(s_sceneData.bRendering == false, "Scene already ready for rendering");
			s_sceneData.bRendering = true;

			s_xShader->Bind();
			glm::mat4 mViewProj = _mProjectionMatrix * _mViewMatrix;
			s_xShader->SetUniformMat4("u_mViewProjection", mViewProj);
		}

		void EndScene()
		{
			ASSERTM(s_sceneData.bRendering == true, "Scene not ready for rendering");

			if (s_sceneData.uNbQuad > 0)
				DrawBatch();

			s_sceneData.bRendering = false;
		}

		void Clear()
		{
			s_pAPI->Clear();
			s_xFramebuffer->ClearRT();
		}

		void SubmitDraw(const MeshPtr &_xMesh)
		{
			_xMesh->m_xShader->Bind();
			_xMesh->m_xShader->SetUniformMat4("u_mViewProjection", s_sceneData.mVPMatrix); // will have to move into scene prepare command

			_xMesh->m_xShader->SetUniformMat4("u_mModel", _xMesh->m_mTransform);
			if (_xMesh->m_hTex)
			{
				_xMesh->m_xShader->SetUniformInt("u_tDiffuse", 0);
				_xMesh->m_hTex->BindSlot(0);
			}

			_xMesh->m_xVertexArray->Bind();
			s_pAPI->DrawIndexed(_xMesh->m_xVertexArray);
		}

		ShaderPtr LoadShader(const StringPath &_sFile)
		{
			return s_shaderLibrary.Load(_sFile);
		}


		void PushEntityId(entt::id_type _entityId)
		{
			s_sceneData.currentEntityId.push(_entityId);
		}

		void PopEntityId()
		{
			s_sceneData.currentEntityId.pop();
		}

		entt::id_type GetEntityId(const glm::ivec2 &_vCoord)
		{
			if (s_xFramebuffer)
			{
				return s_xFramebuffer->ReadPixelUInt32(1, _vCoord.x, _vCoord.y);
			}

			return {};
		}

		void DrawQuad(const glm::vec4 &_vColor, const glm::vec3 &_vPos, const glm::vec2 &_vSize)
		{
			glm::mat4 mTransform = glm::translate(glm::identity<glm::mat4>(), _vPos);
			mTransform			 = glm::scale(mTransform, glm::vec3(_vSize, 1.f));

			DrawQuad(_vColor, mTransform);
		}

		void DrawQuad(const glm::vec4 &_vColor, const glm::vec3 &_vPos, const glm::vec2 &_vSize, float _fAngleRad)
		{
			glm::mat4 mTransform = glm::translate(glm::identity<glm::mat4>(), _vPos);
			mTransform			 = glm::rotate(mTransform, _fAngleRad, glm::vec3(0.f, 0.f, 1.f));
			mTransform			 = glm::scale(mTransform, glm::vec3(_vSize, 1.f));

			DrawQuad(_vColor, mTransform);
		}

		void DrawQuad(const glm::vec4 &_vColor, const glm::mat4 &_mTransform)
		{
			PROFILING_FUNCTION();

			ASSERTM(s_sceneData.bRendering, "Renderer::PrepareScene has not been called");


			if (s_sceneData.uNbQuad == MAX_QUADS || s_sceneData.uNbTex == MAX_TEX_UNIT)
				DrawBatch();

			Vertex v;
			v.vColor = _vColor;
			if (!s_sceneData.currentEntityId.empty())
				v.fEntityId = (float)s_sceneData.currentEntityId.top();

			bool bTexFound = false;
			for (uint32_t i = 0; i < s_sceneData.uNbTex; ++i)
			{
				if (s_sceneData.pTextureUnits[i]->Compare(*s_defaultTextures[(int)DefaultTexture::WHITE]))
				{
					v.fTexUnit = (float)i;
					bTexFound  = true;
					break;
				}
			}
			if (!bTexFound)
			{
				s_sceneData.pTextureUnits[s_sceneData.uNbTex] = s_defaultTextures[(int)DefaultTexture::WHITE].get();
				v.fTexUnit									  = (float)s_sceneData.uNbTex++;
			}

			v.vPosition = _mTransform * glm::vec4(-0.5f, -0.5f, 0.f, 1.f);
			v.vUV		= glm::vec2(0.f);
			if constexpr (Renderer::USE_INVERTED_Y_UV)
				v.vUV.y = 1.f;
			s_sceneData.vecVertices.push_back(v);
			v.vPosition = _mTransform * glm::vec4(0.5f, -0.5f, 0.f, 1.f);
			v.vUV.x		= 1.f;
			s_sceneData.vecVertices.push_back(v);
			v.vPosition = _mTransform * glm::vec4(0.5f, 0.5f, 0.f, 1.f);
			v.vUV.y		= 1.f;
			if constexpr (Renderer::USE_INVERTED_Y_UV)
				v.vUV.y = 0.f;
			s_sceneData.vecVertices.push_back(v);
			v.vPosition = _mTransform * glm::vec4(-0.5f, 0.5f, 0.f, 1.f);
			v.vUV.x		= 0.f;
			s_sceneData.vecVertices.push_back(v);


			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 0);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 1);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 2);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 2);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 3);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 0);

			++s_sceneData.uNbQuad;
		}

		void DrawTexture(const Texture &_Texture, const glm::vec3 &_vPos, const glm::vec2 &_vSize, const glm::vec4 &_vTint /*= glm::vec4( 1.f )*/)
		{
			glm::mat4 mTransform = glm::translate(glm::identity<glm::mat4>(), _vPos);
			mTransform			 = glm::scale(mTransform, glm::vec3(_vSize, 1.f));

			DrawTexture(_Texture, mTransform, _vTint);
		}

		void DrawTexture(const Texture &_Texture, const glm::vec3 &_vPos, const glm::vec2 &_vSize, float _fAngleRad,
						 const glm::vec4 &_vTint /*= glm::vec4( 1.f )*/)
		{
			glm::mat4 mTransform = glm::translate(glm::identity<glm::mat4>(), _vPos);
			mTransform			 = glm::rotate(mTransform, _fAngleRad, glm::vec3(0.f, 0.f, 1.f));
			mTransform			 = glm::scale(mTransform, glm::vec3(_vSize, 1.f));

			DrawTexture(_Texture, mTransform, _vTint);
		}

		void DrawTexture(const Texture &_Texture, const glm::mat4 &_mTransform, const glm::vec4 &_vTint /*= glm::vec4( 1.f )*/)
		{
			PROFILING_FUNCTION();

			ASSERTM(s_sceneData.bRendering, "Renderer::PrepareScene has not been called");


			if (s_sceneData.uNbQuad == MAX_QUADS || s_sceneData.uNbTex == MAX_TEX_UNIT)
				DrawBatch();

			Vertex v;
			v.vColor	= _vTint;
			v.fEntityId = (float)s_sceneData.currentEntityId.top();

			bool bTexFound = false;
			for (uint32_t i = 0; i < s_sceneData.uNbTex; ++i)
			{
				if (s_sceneData.pTextureUnits[i]->Compare(_Texture))
				{
					v.fTexUnit = (float)i;
					bTexFound  = true;
					break;
				}
			}
			if (!bTexFound)
			{
				s_sceneData.pTextureUnits[s_sceneData.uNbTex] = &_Texture;
				v.fTexUnit									  = (float)s_sceneData.uNbTex++;
			}

			v.vPosition = _mTransform * glm::vec4(-0.5f, -0.5f, 0.f, 1.f);
			v.vUV		= glm::vec2(0.f);
			if constexpr (Renderer::USE_INVERTED_Y_UV)
				v.vUV.y = 1.f;
			s_sceneData.vecVertices.push_back(v);
			v.vPosition = _mTransform * glm::vec4(0.5f, -0.5f, 0.f, 1.f);
			v.vUV.x		= 1.f;
			s_sceneData.vecVertices.push_back(v);
			v.vPosition = _mTransform * glm::vec4(0.5f, 0.5f, 0.f, 1.f);
			v.vUV.y		= 1.f;
			if constexpr (Renderer::USE_INVERTED_Y_UV)
				v.vUV.y = 0.f;
			s_sceneData.vecVertices.push_back(v);
			v.vPosition = _mTransform * glm::vec4(-0.5f, 0.5f, 0.f, 1.f);
			v.vUV.x		= 0.f;
			s_sceneData.vecVertices.push_back(v);


			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 0);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 1);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 2);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 2);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 3);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 0);

			++s_sceneData.uNbQuad;
		}

		void DrawSprite(const Sprite &_sprite, const glm::vec3 &_vPos, const glm::vec2 &_vSize /*= glm::vec2( 1.f )*/, float _fAngleRad /*= 0.f*/)
		{
			glm::mat4 mTransform = glm::translate(glm::identity<glm::mat4>(), _vPos);
			mTransform			 = glm::rotate(mTransform, _fAngleRad, glm::vec3(0.f, 0.f, 1.f));
			mTransform			 = glm::scale(mTransform, glm::vec3(_vSize, 1.f));

			DrawSprite(_sprite, mTransform);
		}

		void DrawSprite(const Sprite &_sprite, const glm::mat4 &_mTransform)
		{
			PROFILING_FUNCTION();

			ASSERTM(s_sceneData.bRendering, "Renderer::PrepareScene has not been called");


			if (s_sceneData.uNbQuad == MAX_QUADS || s_sceneData.uNbTex == MAX_TEX_UNIT)
				DrawBatch();

			Vertex v;
			v.vColor	= _sprite.GetTint();
			v.fEntityId = (float)s_sceneData.currentEntityId.top();

			bool bTexFound = false;
			for (uint32_t i = 0; i < s_sceneData.uNbTex; ++i)
			{
				if (_sprite.GetTextureHandle())
				{
					if (s_sceneData.pTextureUnits[i]->Compare(_sprite.GetTextureHandle()))
					{
						v.fTexUnit = (float)i;
						bTexFound  = true;
						break;
					}
				}
				else // default to white texture
				{
					if (s_sceneData.pTextureUnits[i]->Compare(*s_defaultTextures[(int)DefaultTexture::WHITE]))
					{
						v.fTexUnit = (float)i;
						bTexFound  = true;
						break;
					}
				}
			}
			if (!bTexFound)
			{
				if (_sprite.GetTextureHandle())
				{
					s_sceneData.pTextureUnits[s_sceneData.uNbTex] = &(*_sprite.GetTextureHandle());
					v.fTexUnit									  = (float)s_sceneData.uNbTex++;
				}
				else // default to white texture
				{
					s_sceneData.pTextureUnits[s_sceneData.uNbTex] = s_defaultTextures[(int)DefaultTexture::WHITE].get();
					v.fTexUnit									  = (float)s_sceneData.uNbTex++;
				}
			}

			v.vPosition = _mTransform * glm::vec4(-0.5f, -0.5f, 0.f, 1.f);
			v.vUV		= _sprite.GetUVMin();
			if constexpr (Renderer::USE_INVERTED_Y_UV)
				v.vUV.y = _sprite.GetUVMax().y;
			s_sceneData.vecVertices.push_back(v);
			v.vPosition = _mTransform * glm::vec4(0.5f, -0.5f, 0.f, 1.f);
			v.vUV.x		= _sprite.GetUVMax().x;
			s_sceneData.vecVertices.push_back(v);
			v.vPosition = _mTransform * glm::vec4(0.5f, 0.5f, 0.f, 1.f);
			v.vUV		= _sprite.GetUVMax();
			if constexpr (Renderer::USE_INVERTED_Y_UV)
				v.vUV.y = _sprite.GetUVMin().y;
			s_sceneData.vecVertices.push_back(v);
			v.vPosition = _mTransform * glm::vec4(-0.5f, 0.5f, 0.f, 1.f);
			v.vUV.x		= _sprite.GetUVMin().x;
			s_sceneData.vecVertices.push_back(v);

			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 0);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 1);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 2);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 2);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 3);
			s_sceneData.vecIndices.push_back(s_sceneData.uNbQuad * 4 + 0);

			++s_sceneData.uNbQuad;
		}

		void DrawBatch()
		{
			PROFILING_FUNCTION();

			ASSERTM(s_sceneData.bRendering, "Renderer::PrepareScene has not been called");

			s_xVertexBuffer->SetData(s_sceneData.vecVertices.size() * sizeof(Vertex), (float *)s_sceneData.vecVertices.data());
			s_xIndexBuffer->SetData((uint32_t)s_sceneData.vecIndices.size(), s_sceneData.vecIndices.data());

			for (uint32_t i = 0; i < s_sceneData.uNbTex; ++i)
			{
				s_sceneData.pTextureUnits[i]->BindSlot(i);
			}

			s_xVertexArray->Bind();
			s_pAPI->DrawIndexed(s_xVertexArray);

			s_sceneData.frameStat.uDrawCalls++;
			s_sceneData.frameStat.uQuads += s_sceneData.uNbQuad;

			s_sceneData.uNbQuad = 0;
			s_sceneData.uNbTex	= 0;
			s_sceneData.vecVertices.clear();
			s_sceneData.vecIndices.clear();
		}

		uint64_t GetDefaultTextureHandle(DefaultTexture _eTexture)
		{
			TexturePtr xTex = s_defaultTextures[(int)_eTexture];
			ASSERT((bool)xTex);
			return xTex->GetNativeHandle();
		}

		FrameBufferPtr GetFrameBuffer()
		{
			return s_xFramebuffer;
		}


		RenderAPIPtr Create()
		{
			switch (Renderer::GetAPI())
			{
				case RenderAPI::API::OPENGL: return std::make_shared<OpenGL_Renderer>();
			}

			ASSERTM(false, "Render API not supported");
			return nullptr;
		}

	} // namespace Renderer
} // namespace Plop
