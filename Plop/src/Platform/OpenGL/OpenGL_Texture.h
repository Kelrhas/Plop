#pragma once

#include <Gl/glew.h>

#include <Renderer/Texture.h>

namespace Plop
{
	class OpenGL_Texture2D : public Texture2D
	{
	public:
		OpenGL_Texture2D( const String& _sFile );
		OpenGL_Texture2D( uint32_t _uWidth, uint32_t _uHeight, void* _pData );
		virtual ~OpenGL_Texture2D();

		// from Texture
		virtual void BindSlot( int _iSlot ) const override;
		virtual void SetData( void* _pData ) override;

		virtual bool Compare( const Texture& _other ) override;

	private:
		GLuint m_uID;
		GLenum m_eDataFormat;
	};
}
