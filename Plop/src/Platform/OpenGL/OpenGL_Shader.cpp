#include "Plop_pch.h"
#include "OpenGL_Shader.h"

#include <GL/glew.h>
#include <GL/GL.h>

namespace Plop
{

	OpenGL_Shader::OpenGL_Shader()
	{

	}

	OpenGL_Shader::~OpenGL_Shader()
	{
		glDeleteShader( m_uVertShader );
		glDeleteShader( m_uFragShader );
		glDeleteProgram( m_uProgram );
	}

	void OpenGL_Shader::Create( const String& _sVertSrc, const String& _sFragSrc )
	{
		GLint iIsCompiled;

		//////////////////////////////////////////////////////////////////////////
		// VERTEX

		m_uVertShader = glCreateShader( GL_VERTEX_SHADER );
		const GLchar* pVertSrc = (const GLchar*)_sVertSrc.c_str();
		glShaderSource( m_uVertShader, 1, &pVertSrc, nullptr );
		glCompileShader( m_uVertShader );

		glGetShaderiv( m_uVertShader, GL_COMPILE_STATUS, &iIsCompiled );
		if (iIsCompiled == GL_FALSE)
		{
			GLint iMaxLength = 0;
			glGetShaderiv(m_uVertShader, GL_INFO_LOG_LENGTH, &iMaxLength);

			GLchar* pLog = new GLchar[iMaxLength];
			glGetShaderInfoLog(m_uVertShader, iMaxLength, &iMaxLength, &pLog[0]);

			Log::Error( "Vertex shader failed to compile:\n%s", pLog);

			glDeleteShader( m_uVertShader );
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		// FRAGMENT

		m_uFragShader = glCreateShader( GL_FRAGMENT_SHADER );
		const GLchar* pFragSrc = (const GLchar*)_sFragSrc.c_str();
		glShaderSource( m_uFragShader, 1, &pFragSrc, nullptr );
		glCompileShader( m_uFragShader );

		glGetShaderiv( m_uFragShader, GL_COMPILE_STATUS, &iIsCompiled );
		if (iIsCompiled == GL_FALSE)
		{
			GLint iMaxLength = 0;
			glGetShaderiv(m_uFragShader, GL_INFO_LOG_LENGTH, &iMaxLength);

			GLchar* pLog = new GLchar[iMaxLength];
			glGetShaderInfoLog(m_uFragShader, iMaxLength, &iMaxLength, &pLog[0]);

			Log::Error( "Fragment shader failed to compile:\n%s", pLog);

			glDeleteShader( m_uVertShader );
			glDeleteShader( m_uFragShader );
			return;
		}


		//////////////////////////////////////////////////////////////////////////
		// PROGRAM

		m_uProgram = glCreateProgram();
		glAttachShader( m_uProgram, m_uVertShader );
		glAttachShader( m_uProgram, m_uFragShader );
		glLinkProgram( m_uProgram );

		GLint iIsLinked;
		glGetProgramiv( m_uProgram, GL_LINK_STATUS, &iIsLinked );
		if (iIsLinked == GL_FALSE)
		{
			Log::Error( "Program did not link" );
			glDeleteShader( m_uVertShader );
			glDeleteShader( m_uFragShader );
			glDeleteProgram( m_uFragShader );
		}
	}

	void OpenGL_Shader::Bind() const
	{
		glUseProgram( m_uProgram );
	}

	void OpenGL_Shader::Unbind() const
	{
		glUseProgram( 0 );
	}
}
