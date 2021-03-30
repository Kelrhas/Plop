#include "Plop_pch.h"
#include "OpenGL_FrameBuffer.h"

#include <GL/glew.h>

namespace Plop
{
	OpenGL_FrameBuffer::OpenGL_FrameBuffer( uint32_t _uWidth, uint32_t _uHeight )
		: FrameBuffer( _uWidth, _uHeight )
	{
		Recreate();
	}

	OpenGL_FrameBuffer::~OpenGL_FrameBuffer()
	{
		glDeleteTextures( 1, &m_uColorBuffer );
		glDeleteTextures( 1, &m_uDepthBuffer );
		glDeleteFramebuffers( 1, &m_uID );
	}
	
	void OpenGL_FrameBuffer::Bind()
	{
		glBindFramebuffer( GL_FRAMEBUFFER, m_uID );
	}

	void OpenGL_FrameBuffer::Unbind()
	{
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void OpenGL_FrameBuffer::Resize( uint32_t _uWidth, uint32_t _uHeight )
	{
		glDeleteTextures( 1, &m_uColorBuffer );
		glDeleteTextures( 1, &m_uDepthBuffer );
		glDeleteFramebuffers( 1, &m_uID );

		m_uWidth = _uWidth;
		m_uHeight = _uHeight;

		Recreate();
	}

	void OpenGL_FrameBuffer::Recreate()
	{
		glCreateFramebuffers( 1, &m_uID );
		glBindFramebuffer( GL_FRAMEBUFFER, m_uID );

		// COLOR
		glCreateTextures( GL_TEXTURE_2D, 1, &m_uColorBuffer );
		glBindTexture( GL_TEXTURE_2D, m_uColorBuffer );
		glTextureStorage2D( m_uColorBuffer, 1, GL_RGBA8, m_uWidth, m_uHeight );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_uColorBuffer, 0 );

		// DEPTH
		glCreateTextures( GL_TEXTURE_2D, 1, &m_uDepthBuffer );
		glBindTexture( GL_TEXTURE_2D, m_uDepthBuffer );
		glTextureStorage2D( m_uDepthBuffer, 1, GL_DEPTH24_STENCIL8, m_uWidth, m_uHeight );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_uDepthBuffer, 0 );


		ASSERTM( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE, "Error with FrameBuffer creation" );


		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}
