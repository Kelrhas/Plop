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
	//class Renderer2D;
	namespace Renderer
	{
		//friend class Renderer2D;

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
			bool					bRendering = false;
			glm::mat4				mVPMatrix = glm::identity<glm::mat4>();
			std::vector<Vertex>		vecVertices;
			std::vector<uint32_t>	vecIndices;
			uint32_t				uNbQuad = 0;
			uint32_t				uNbTex = 0;
			const Texture**			pTextureUnits = nullptr; // [MAX_TEX_UNIT]
			Stats					frameStat;
		};


		void			Init();

		void			OnResize(uint32_t _uWidth, uint32_t _uHeight);

		void			NewFrame();
		void			EndFrame();

		void			PrepareScene( const glm::mat4& _mProjectionMatrix, const glm::mat4& _mViewMatrix );
		void			EndScene();

		void			Clear();

		void			SubmitDraw(const MeshPtr& _xMesh);

		ShaderPtr		LoadShader( const String& _sFile );



		void			DrawQuad( const glm::vec4& _vColor, const glm::vec2& _vPos, const glm::vec2& _vSize );
		void			DrawQuad( const glm::vec4& _vColor, const glm::vec2& _vPos, const glm::vec2& _vSize, float _fAngleRad );
		void			DrawQuad( const glm::vec4& _vColor, const glm::mat4& _mTransform );

		void			DrawTexture( const Texture& _Texture, const glm::vec2& _vPos, const glm::vec2& _vSize, const glm::vec4& _vTint = glm::vec4( 1.f ) );
		void			DrawTexture( const Texture& _Texture, const glm::vec2& _vPos, const glm::vec2& _vSize, float _fAngleRad, const glm::vec4& _vTint = glm::vec4( 1.f ) );
		void			DrawTexture( const Texture& _Texture, const glm::mat4& _mTransform, const glm::vec4& _vTint = glm::vec4( 1.f ) );

		void			DrawSprite( const Sprite& _sprite, const glm::vec2& _vPos, const glm::vec2& _vSize = glm::vec2( 1.f ), float _fAngleRad = 0.f );
		void			DrawSprite( const Sprite& _sprite, const glm::mat4& _mTransform );


		enum class DefaultTexture : uint8_t
		{
			WHITE,
			CHECKER,

			COUNT
		};
		uint64_t		GetDefaultTextureHandle( DefaultTexture _eTexture );

		inline RenderAPI::API	GetAPI() { return RenderAPI::GetAPI(); }

	};
}