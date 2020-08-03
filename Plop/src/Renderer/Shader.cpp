#include "Plop_pch.h"
#include "Shader.h"

#include <Platform/OpenGL/OpenGL_Shader.h>
#include <Renderer/Renderer.h>

namespace Plop
{
	ShaderPtr Shader::Create(const String& _sFile)
	{
		ShaderPtr xShader = nullptr;
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL:		xShader = std::make_shared < OpenGL_Shader>();
		}

		if (xShader)
		{
			xShader->Load(_sFile);
			return xShader;
		}


		ASSERT(false, "Render API not supported");
		return nullptr;
	}

	ShaderPtr Shader::Create(const String& _sVertSrc, const String& _sFragSrc)
	{
		ShaderPtr xShader = nullptr;
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL:		xShader = std::make_shared<OpenGL_Shader>();
		}

		if (xShader)
		{
			xShader->Load(_sVertSrc, _sFragSrc);
			return xShader;
		}


		ASSERT(false, "Render API not supported");
		return nullptr;
	}

}
