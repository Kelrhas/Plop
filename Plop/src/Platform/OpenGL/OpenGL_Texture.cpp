#include "Plop_pch.h"
#include "OpenGL_Texture.h"

#include <stb_image.h>

namespace Plop
{
	OpenGL_Texture2D::OpenGL_Texture2D( const String& _sFile )
		: Texture2D()
	{
		int iWidth, iHeight, iChannels;
		stbi_set_flip_vertically_on_load( true );
		stbi_uc* pData = stbi_load( _sFile.c_str(), &iWidth, &iHeight, &iChannels, 0 );
		ASSERT( pData, "File '%s' has failed to load", _sFile.c_str() );
		ASSERT( iChannels == 4, "Check nuber of channels in GL upload" );

		m_uWidth = iWidth;
		m_uHeight = iHeight;

		glCreateTextures( GL_TEXTURE_2D, 1, &m_uID );
		glTextureStorage2D( m_uID, 1, GL_RGBA8, m_uWidth, m_uHeight );

		glTextureParameteri( m_uID, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTextureParameteri( m_uID, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glTextureSubImage2D( m_uID, 0, 0, 0, m_uWidth, m_uHeight, GL_RGBA, GL_UNSIGNED_BYTE, pData );

		stbi_image_free( pData );
	}


	OpenGL_Texture2D::~OpenGL_Texture2D()
	{
		glDeleteTextures( 1, &m_uID );
	}

	void OpenGL_Texture2D::BindSlot(int _iSlot) const
	{
		glBindTexture( GL_TEXTURE_2D, m_uID );
	}
}