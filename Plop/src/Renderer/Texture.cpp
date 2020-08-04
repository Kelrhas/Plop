#include "Plop_pch.h"
#include "Texture.h"

#include <Platform/OpenGL/OpenGL_Texture.h>
#include <Renderer/Renderer.h>

namespace Plop
{
	TexturePtr Texture::Create2D( const String& _sFile )
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL:		return std::make_shared<OpenGL_Texture2D>(_sFile);
		}
		ASSERT( false, "Render API not supported" );
		return nullptr;
	}
}