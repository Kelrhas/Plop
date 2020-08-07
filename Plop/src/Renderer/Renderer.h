#pragma once

#include <Renderer/Mesh.h>
#include <Camera/Camera.h>
#include <Camera/OrthographicCamera.h>

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

		static void				PrepareScene(const Camera& _Camera);
		static void				EndScene();

		static void				Clear();

		static void				SubmitDraw(const MeshPtr& _xMesh);

		static ShaderPtr		LoadShader( const String& _sFile );


		static RenderAPI::API	GetAPI() { return RenderAPI::GetAPI(); }

	private:
		static RenderAPI*		s_pAPI;
		static SceneData		s_SceneData;
		static ShaderLibrary	s_shaderLibrary;
	};

	//////////////////////////////////////////////////////////////////////////
	// Renderer2D
	class Renderer2D
	{
		struct SceneData
		{
			glm::mat4 mVPMatrix;
		};

	public:
		static void				Init();

		static void				PrepareScene( const OrthographicCamera& _camera );
		static void				EndScene();

		static void				Clear();

		static void				DrawQuadColor( glm::vec2 _vPos, glm::vec2 _vSize, glm::vec4 _vColor );
		static void				DrawQuadColor( glm::vec3 _vPos, glm::vec2 _vSize, glm::vec4 _vColor );

	private:
		static bool				s_bRendering2D;
		static Mesh				s_Quad;
	};

}