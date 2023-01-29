#include "Plop_pch.h"
#include "OpenGL_FrameBuffer.h"

#include <GL/glew.h>
#include <entt/entt.hpp> // for entt::null

#include "OpenGL_Debug.h"

namespace
{
	constexpr GLenum GetGLInternalFormatFromSpecFormat(Plop::FrameBuffer::TextureFormat _eFormat)
	{
		switch (_eFormat)
		{
			case Plop::FrameBuffer::TextureFormat::RGBA8:		return GL_RGBA8;
			case Plop::FrameBuffer::TextureFormat::UINT32:		return GL_R32UI;
			case Plop::FrameBuffer::TextureFormat::DEPTH24S8:	return GL_DEPTH24_STENCIL8;
		}
		return GL_NONE;
	}
	constexpr GLenum GetGLFormatFromSpecFormat(Plop::FrameBuffer::TextureFormat _eFormat)
	{
		//return GL_RGBA;

		switch (_eFormat)
		{
			case Plop::FrameBuffer::TextureFormat::RGBA8:		return GL_RGBA;
			case Plop::FrameBuffer::TextureFormat::UINT32:		return GL_RED_INTEGER;
			case Plop::FrameBuffer::TextureFormat::DEPTH24S8:	return GL_DEPTH_STENCIL;
		}
		return GL_NONE;
	}
	constexpr GLenum GetGLTypeFromSpecFormat(Plop::FrameBuffer::TextureFormat _eFormat)
	{
		//return GL_FLOAT;
		//return GL_UNSIGNED_BYTE;

		switch (_eFormat)
		{
			case Plop::FrameBuffer::TextureFormat::RGBA8:		return GL_FLOAT;
			case Plop::FrameBuffer::TextureFormat::UINT32:		return GL_UNSIGNED_INT;
			case Plop::FrameBuffer::TextureFormat::DEPTH24S8:	return GL_FLOAT;
		}
		return GL_NONE;
	}

	constexpr bool IsDepthFormat(Plop::FrameBuffer::TextureFormat _eFormat)
	{
		switch (_eFormat)
		{
			case Plop::FrameBuffer::TextureFormat::DEPTH24S8:	return true;
		}
		return false;
	}
}

namespace Plop
{
	OpenGL_FrameBuffer::OpenGL_FrameBuffer(const Specification &_specs)
		: FrameBuffer( _specs )
	{

		for (const auto &spec : _specs.vecRenderTargetSpecs)
		{
			if (IsDepthFormat(spec.eFormat))
			{
				ASSERTM(m_depthSpec.eFormat == TextureFormat::NONE, "There already is a depth format");
				m_depthSpec = spec;
			}
			else
			{
				m_vecColorSpecs.push_back(spec);
			}
		}

		Recreate();
	}

	OpenGL_FrameBuffer::~OpenGL_FrameBuffer()
	{
		for(auto id : m_vecColorAttachments)
			glDeleteTextures( 1, &id );

		if (m_uDepthAttachment)
			glDeleteTextures(1, &m_uDepthAttachment);

		glDeleteFramebuffers( 1, &m_uID );
	}
	
	void OpenGL_FrameBuffer::Bind()
	{
		GL_DEBUG_MARKER( "FrameBuffer::Bind" );

		glBindFramebuffer( GL_FRAMEBUFFER, m_uID );
		glViewport( 0, 0, m_specs.uWidth, m_specs.uHeight );
	}

	void OpenGL_FrameBuffer::Unbind()
	{
		GL_DEBUG_MARKER( "FrameBuffer::Unbind" );

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void OpenGL_FrameBuffer::Resize( uint32_t _uWidth, uint32_t _uHeight )
	{
		for (auto id : m_vecColorAttachments)
			glDeleteTextures(1, &id);
		m_vecColorAttachments.clear();

		if (m_uDepthAttachment)
			glDeleteTextures(1, &m_uDepthAttachment);
		m_uDepthAttachment = 0;

		glDeleteFramebuffers( 1, &m_uID );
		m_uID = 0;

		m_specs.uWidth = _uWidth;
		m_specs.uHeight = _uHeight;

		Recreate();
	}

	uint32_t OpenGL_FrameBuffer::ReadPixelUInt32(uint32_t _renderTargetIndex, int _x, int _y)
	{
		GL_DEBUG_MARKER("FrameBuffer::ReadPixel");

		//glNamedFramebufferReadBuffer(m_uID, GL_COLOR_ATTACHMENT0 + _renderTargetIndex);
		Bind();
		glReadBuffer(GL_COLOR_ATTACHMENT0 + _renderTargetIndex);
		Debug::Assert_GL();

		uint32_t pixelData;
		glReadPixels(_x, m_specs.uHeight - _y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixelData);
		Debug::Assert_GL();

		return pixelData;
	}

	void OpenGL_FrameBuffer::ClearRT()
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, m_uID);
		const GLuint id = (GLuint)entt::null;
		glClearBufferuiv(GL_COLOR, 1, &id);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_FrameBuffer::Recreate()
	{
		GL_DEBUG_GROUP_SCOPED("FrameBuffer::Recreate");

		glCreateFramebuffers( 1, &m_uID );
		glBindFramebuffer( GL_FRAMEBUFFER, m_uID );
		glObjectLabel( GL_FRAMEBUFFER, m_uID, 0, "Framebuffer" );
		Debug::Assert_GL();


		ASSERTM(m_vecColorAttachments.empty(), "Color attachments were not cleared");
		m_vecColorAttachments.resize(m_vecColorSpecs.size());
		if (!m_vecColorAttachments.empty())
		{
			glCreateTextures(GL_TEXTURE_2D, (int)m_vecColorAttachments.size(), m_vecColorAttachments.data());

			for (size_t i = 0; i < m_vecColorSpecs.size(); ++i)
			{
				const auto &uAttachment = m_vecColorAttachments[i];
				const auto &spec = m_vecColorSpecs[i];

				glBindTexture(GL_TEXTURE_2D, uAttachment);
				glTexStorage2D(GL_TEXTURE_2D, 1,
							   GetGLInternalFormatFromSpecFormat(spec.eFormat),
							   m_specs.uWidth, m_specs.uHeight);
				//glTexImage2D(GL_TEXTURE_2D, 0,
				//				GetGLInternalFormatFromSpecFormat(spec.eFormat),
				//				m_specs.uWidth, m_specs.uHeight, 0,
				//				GetGLFormatFromSpecFormat(spec.eFormat),
				//				GetGLTypeFromSpecFormat(spec.eFormat),
				//				nullptr);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (int)i, GL_TEXTURE_2D, uAttachment, 0);
				glObjectLabel(GL_TEXTURE, uAttachment, 0, "Framebuffer color attachment");
				Debug::Assert_GL();
			}
		}


		if (m_depthSpec.eFormat != TextureFormat::NONE)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_uDepthAttachment);
			glBindTexture(GL_TEXTURE_2D, m_uDepthAttachment);
			glTexStorage2D(GL_TEXTURE_2D, 1, GetGLInternalFormatFromSpecFormat(m_depthSpec.eFormat), m_specs.uWidth, m_specs.uHeight);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_uDepthAttachment, 0);
			glObjectLabel(GL_TEXTURE, m_uDepthAttachment, 0, "Framebuffer depth attachment");
			Debug::Assert_GL();
		}


		if (m_vecColorAttachments.size() >= 2)
		{
			static const GLenum attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
			glDrawBuffers((GLsizei)m_vecColorAttachments.size(), attachments);
		}
		else if (m_vecColorAttachments.empty())
		{
			glDrawBuffer(GL_NONE);
		}

		ASSERTM( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE, "Error with FrameBuffer creation" );

		Debug::Assert_GL();
		//glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}
}
