#include "Plop_pch.h"
#include "Shader.h"

#include <Platform/OpenGL/OpenGL_Shader.h>
#include <Renderer/Renderer.h>

namespace Plop
{
	Shader* Shader::Create(const String& _sVertSrc, const String& _sFragSrc)
	{
		Shader* pShader = nullptr;
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL:		pShader = new OpenGL_Shader();
		}

		if (pShader)
		{
			pShader->Load(_sVertSrc, _sFragSrc);
			return pShader;
		}


		ASSERT(false, "Render API not supported");
		return nullptr;
	}

}
