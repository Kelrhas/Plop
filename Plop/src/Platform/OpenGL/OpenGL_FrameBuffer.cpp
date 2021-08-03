#include "Plop_pch.h"
#include "OpenGL_FrameBuffer.h"

#include <GL/glew.h>

#include "OpenGL_Debug.h"

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
		GL_DEBUG_MARKER( "Binding framebuffer" );
		glBindFramebuffer( GL_FRAMEBUFFER, m_uID );
	}

	void OpenGL_FrameBuffer::Unbind()
	{
		GL_DEBUG_MARKER( "Unbinding framebuffer" );
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
		GL_DEBUG_GROUP_SCOPED( "Creating framebuffer" );
		glCreateFramebuffers( 1, &m_uID );
		glBindFramebuffer( GL_FRAMEBUFFER, m_uID );
		glObjectLabel( GL_FRAMEBUFFER, m_uID, 0, "Framebuffer" );

		// COLOR
		glCreateTextures( GL_TEXTURE_2D, 1, &m_uColorBuffer );
		glBindTexture( GL_TEXTURE_2D, m_uColorBuffer );
		glTextureStorage2D( m_uColorBuffer, 1, GL_RGBA8, m_uWidth, m_uHeight );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_uColorBuffer, 0 );
		glObjectLabel( GL_TEXTURE, m_uColorBuffer, 0, "Framebuffer color" );

		// DEPTH
		glCreateTextures( GL_TEXTURE_2D, 1, &m_uDepthBuffer );
		glBindTexture( GL_TEXTURE_2D, m_uDepthBuffer );
		glTextureStorage2D( m_uDepthBuffer, 1, GL_DEPTH24_STENCIL8, m_uWidth, m_uHeight );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_uDepthBuffer, 0 );
		glObjectLabel( GL_TEXTURE, m_uDepthBuffer, 0, "Framebuffer depth" );


		ASSERTM( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE, "Error with FrameBuffer creation" );


		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}
