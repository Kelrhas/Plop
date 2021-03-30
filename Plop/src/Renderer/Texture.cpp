#include "Plop_pch.h"
#include "Texture.h"

#include <Platform/OpenGL/OpenGL_Texture.h>
#include <Renderer/Renderer.h>

namespace Plop
{
	TexturePtr Texture::Create2D( uint32_t _uWidth, uint32_t _uHeight, FlagsType _eFlags, void* _pData, const String& _sName /*= ""*/ )
	{
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL:		return std::make_shared<OpenGL_Texture2D>( _uWidth, _uHeight, _eFlags, _pData, _sName );
		}
		ASSERTM( false, "Render API not supported" );
		return nullptr;
	}
}