#include "Config.h"

#include "Debug/Debug.h"
#include "OpenGL_Texture.h"

#include <stb_image.h>

namespace Plop
{
	OpenGL_Texture2D::OpenGL_Texture2D(const StringPath &_sFile)
		: Texture2D()
	{
		int iWidth, iHeight, iChannels;
		stbi_set_flip_vertically_on_load( true );
		stbi_uc* pData = stbi_load( _sFile.string().c_str(), &iWidth, &iHeight, &iChannels, 0 );
		ASSERTM( pData, "Texture file '{}' has failed to load", _sFile.string().c_str() );
		if (!pData)
			return;

		m_uWidth = iWidth;
		m_uHeight = iHeight;

		GLenum eInternalFormat = 0;
		if (iChannels == 4)
		{
			eInternalFormat = GL_RGBA8;
			m_eDataFormat = GL_RGBA;
		}
		else if (iChannels == 3)
		{
			eInternalFormat = GL_RGB8;
			m_eDataFormat = GL_RGB;
		}
		else if (iChannels == 1)
		{
			eInternalFormat = GL_R32F;
			m_eDataFormat = GL_RED;
		}

		glCreateTextures( GL_TEXTURE_2D, 1, &m_uID );
		glTextureStorage2D( m_uID, 1, eInternalFormat, m_uWidth, m_uHeight );

		glTextureParameteri( m_uID, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTextureParameteri( m_uID, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		glTextureParameteri( m_uID, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTextureParameteri( m_uID, GL_TEXTURE_WRAP_T, GL_CLAMP );

		glTextureSubImage2D( m_uID, 0, 0, 0, m_uWidth, m_uHeight, m_eDataFormat, GL_UNSIGNED_BYTE, pData );

		glObjectLabel( GL_TEXTURE, m_uID, 0, _sFile.string().c_str() );

		stbi_image_free( pData );

		m_sName = _sFile.filename().string();
	}

	OpenGL_Texture2D::OpenGL_Texture2D( uint32_t _uWidth, uint32_t _uHeight, FlagsType _eFlags, void* _pData, const String& _sName /*= ""*/ )
		: Texture2D()
	{
		m_uWidth = _uWidth;
		m_uHeight = _uHeight;

		GLenum eInternalFormat = GL_RGBA8;
		m_eDataFormat = GL_RGBA;

		glCreateTextures( GL_TEXTURE_2D, 1, &m_uID );
		glTextureStorage2D( m_uID, 1, eInternalFormat, m_uWidth, m_uHeight );

		glTextureParameteri( m_uID, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTextureParameteri( m_uID, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		if (_eFlags & (FlagsType)Flags::UV_REPEAT)
		{
			glTextureParameteri( m_uID, GL_TEXTURE_WRAP_S, GL_REPEAT );
			glTextureParameteri( m_uID, GL_TEXTURE_WRAP_T, GL_REPEAT );
		}
		else
		{
			glTextureParameteri( m_uID, GL_TEXTURE_WRAP_S, GL_CLAMP );
			glTextureParameteri( m_uID, GL_TEXTURE_WRAP_T, GL_CLAMP );
		}
		
		if (_pData)
			glTextureSubImage2D( m_uID, 0, 0, 0, m_uWidth, m_uHeight, m_eDataFormat, GL_UNSIGNED_BYTE, _pData );


		glObjectLabel(GL_TEXTURE, m_uID, 0, _sName.c_str());
		m_sName = _sName;
	}

	OpenGL_Texture2D::~OpenGL_Texture2D()
	{
		glDeleteTextures( 1, &m_uID );
	}

	void OpenGL_Texture2D::BindSlot(int _iSlot) const
	{
		glBindTextureUnit( _iSlot, m_uID );
	}

	void OpenGL_Texture2D::SetData( void* _pData )
	{
		glTextureSubImage2D( m_uID, 0, 0, 0, m_uWidth, m_uHeight, m_eDataFormat, GL_UNSIGNED_BYTE, _pData );
	}

	bool OpenGL_Texture2D::Compare( const Texture& _other ) const
	{
		return m_uID == ((OpenGL_Texture2D&)_other).m_uID;
	}
}