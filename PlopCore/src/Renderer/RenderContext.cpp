#include "Config.h"

#include "RenderContext.h"

#include "Debug/Debug.h"
#include "Platform/OpenGL/OpenGLWin32_Context.h"
#include "Renderer/Renderer.h"

namespace Plop
{

	RenderContextPtr RenderContext::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL:		return std::make_shared<OpenGLWin32_Context>();
		}

		ASSERTM( false, "Render API not supported" );
		return nullptr;
	}
}