#pragma once

namespace Plop
{
	enum class RenderAPI
	{
		NONE,
		OPENGL,
		//VULKAN,
		//DIRECTX
	};

	class Renderer
	{
	public:
		Renderer();
		~Renderer();


		static RenderAPI GetAPI() { return s_eRenderer; }

	private:
		static RenderAPI s_eRenderer;
	};

}