#include "Config.h"

#include "VertexArray.h"

#include "Debug/Debug.h"
#include "Platform/OpenGL/OpenGL_VertexArray.h"
#include "Renderer/Renderer.h"

namespace Plop
{
	VertexArrayPtr VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL: return std::make_shared<OpenGL_VertexArray>();
		}

		ASSERTM(false, "Render API not supported");
		return nullptr;
	}
}