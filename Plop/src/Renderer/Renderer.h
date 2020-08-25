#pragma once

#include <Renderer/Mesh.h>
#include <Renderer/Sprite.h>
#include <Renderer/FrameBuffer.h>
#include <Camera/Camera.h>

namespace Plop
{

	//////////////////////////////////////////////////////////////////////////
	// RenderAPI
	class RenderAPI
	{
	public:
		enum class API
		{
			NONE,
			OPENGL,
			//VULKAN,
			//DIRECTX
		};

		RenderAPI() = default;
		virtual ~RenderAPI() = default;


		virtual void Init() = 0;
		virtual void Clear() = 0;
		virtual void Resize(uint32_t _uWidth, uint32_t _uHeight) = 0;

		virtual void DrawIndexed(const VertexArrayPtr& _xVertexArray) = 0;
		virtual void DrawFrameBuffer( const FrameBufferPtr& _xFramebuffer ) = 0;
		
		
		virtual int GetMaxTextureUnit() const = 0;

		static API GetAPI() { return s_eRenderer; }

	private:
		static API s_eRenderer;
	};


	//////////////////////////////////////////////////////////////////////////
	// Renderer
	class Renderer2D;
	class Renderer
	{
		friend class Renderer2D;
		struct SceneData
		{
			glm::mat4 mVPMatrix;
		};
	public:
		static void				Init();

		static void				OnResize(uint32_t _uWidth, uint32_t _uHeight);

		static void				PrepareScene( const glm::mat4& _mProjectionMatrix, const glm::mat4& _mViewMatrix );
		static void				EndScene();

		static void				Clear();

		static void				SubmitDraw(const MeshPtr& _xMesh);

		static ShaderPtr		LoadShader( const String& _sFile );


		static RenderAPI::API	GetAPI() { return RenderAPI::GetAPI(); }

	private:
		static RenderAPI*		s_pAPI;
		static SceneData		s_SceneData;
		static ShaderLibrary	s_shaderLibrary;
		static FrameBufferPtr	s_xFramebuffer;
	};

	//////////////////////////////////////////////////////////////////////////
	// Renderer2D
	class Renderer2D
	{
		struct Stats
		{
			uint32_t uDrawCalls = 0;
			uint32_t uQuads = 0;

			void Reset();
		};

		struct Vertex
		{
			glm::vec3	vPosition;
			glm::vec4	vColor;
			glm::vec2	vUV;
			float		fTexUnit; // must be float because it will be varying

			static const BufferLayout layout;
		};

		struct SceneData
		{
			glm::mat4				mVPMatrix = glm::identity<glm::mat4>();
			std::vector<Vertex>		vecVertices;
			std::vector<uint32_t>	vecIndices;
			uint32_t				uNbQuad = 0;
			uint32_t				uNbTex = 0;
			TexturePtr*				pTextureUnits = nullptr;
			Stats					frameStat;
		};


		static uint32_t MAX_TEX_UNIT;
		static const uint32_t MAX_QUADS = 10000;
		static const uint32_t MAX_VERTICES = MAX_QUADS * 4;
		static const uint32_t MAX_INDICES = MAX_QUADS * 6;


	public:
		static void				Init();

		static void				NewFrame();
		static void				EndFrame();

		static void				PrepareScene( const glm::mat4& _mProjectionMatrix, const glm::mat4& _mViewMatrix );
		static void				EndScene();


		static void				DrawQuad( const glm::vec4& _vColor, const glm::vec2& _vPos, const glm::vec2& _vSize );
		static void				DrawQuad( const glm::vec4& _vColor, const glm::vec2& _vPos, const glm::vec2& _vSize, float _fAngleRad );
		static void				DrawQuad( const glm::vec4& _vColor, const glm::mat4& _mTransform );

		static void				DrawTexture( const TexturePtr& _xTexture, const glm::vec2& _vPos, const glm::vec2& _vSize, const glm::vec4& _vTint = glm::vec4( 1.f ) );
		static void				DrawTexture( const TexturePtr& _xTexture, const glm::vec2& _vPos, const glm::vec2& _vSize, float _fAngleRad, const glm::vec4& _vTint = glm::vec4( 1.f ) );
		static void				DrawTexture( const TexturePtr& _xTexture, const glm::mat4& _mTransform, const glm::vec4& _vTint = glm::vec4( 1.f ) );

		static void				DrawSprite( const Sprite& _sprite, const glm::vec2& _vPos, const glm::vec2& _vSize = glm::vec2( 1.f ), float _fAngleRad = 0.f );
		static void				DrawSprite( const Sprite& _sprite, const glm::mat4& _mTransform );

	private:

		static void				DrawBatch();


		static bool				s_bRendering2D;
		static TexturePtr		s_xWhiteTex;
		static ShaderPtr		s_xShader;
		static VertexArrayPtr	s_xVertexArray;
		static VertexBufferPtr	s_xVertexBuffer;
		static IndexBufferPtr	s_xIndexBuffer;

		static SceneData		s_sceneData;
	};

}