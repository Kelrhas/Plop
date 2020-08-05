#pragma once

#include <Renderer/Mesh.h>
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

		virtual void DrawIndexed(const VertexArrayPtr& _xVertexArray) = 0;


		static API GetAPI() { return s_eRenderer; }

	private:
		static API s_eRenderer;
	};


	//////////////////////////////////////////////////////////////////////////
	// Renderer
	class Renderer
	{
	public:
		static void				Init();
		static void				PrepareScene(const Camera* _pCamera);
		static void				EndScene();

		static void				Clear();

		static void				SubmitDraw(const MeshPtr& _xMesh);


		static RenderAPI::API	GetAPI() { return RenderAPI::GetAPI(); }

	private:
		static RenderAPI*		s_pAPI;
		static const Camera*	s_pCurrentCamera;
	};

}