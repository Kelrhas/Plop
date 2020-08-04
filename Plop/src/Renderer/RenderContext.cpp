#include "Plop_pch.h"
#include "RenderContext.h"

#include <Renderer/Renderer.h>
#include <Platform/OpenGL/OpenGLWin32_Context.h>

namespace Plop
{

	RenderContextPtr RenderContext::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL:		return std::make_shared<OpenGLWin32_Context>();
		}

		ASSERT( false, "Render API not supported" );
		return nullptr;
	}
}