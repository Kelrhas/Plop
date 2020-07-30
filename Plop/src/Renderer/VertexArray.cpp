#include <Plop_pch.h>
#include "VertexArray.h"

#include <Renderer/Renderer.h>
#include <Platform/OpenGL/OpenGL_VertexArray.h>

namespace Plop
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::OPENGL: return new OpenGL_VertexArray();
		}

		Log::Assert("Render API not supported");
		return nullptr;
	}
}