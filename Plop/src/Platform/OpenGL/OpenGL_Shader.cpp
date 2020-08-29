#include "Plop_pch.h"
#include "OpenGL_Shader.h"

#include <fstream>
#include <filesystem>

#include <GL/glew.h>
#include <GL/GL.h>

#include <glm/gtc/type_ptr.hpp>

namespace Plop
{

	OpenGL_Shader::OpenGL_Shader()
	{

	}

	OpenGL_Shader::~OpenGL_Shader()
	{
		glDeleteProgram( m_uProgram );
	}

	void OpenGL_Shader::Load(const String& _sFile)
	{
		String sContent;
		std::ifstream file(_sFile, std::ios::in | std::ios::binary);
		ASSERT(file.is_open(), "File not found: %s", _sFile.c_str());
		if (file.is_open())
		{
			file.seekg(0, std::ios::end);
			size_t size = file.tellg();
			ASSERT(size > 0, "The file %s is empty", _sFile);
			if (size > 0)
			{
				sContent.resize(size);
				file.seekg(0);
				file.read(&sContent[0], size);
			}
			file.close();
		}

#ifdef _DEBUG
		// add file watcher to live reload any change
		const char* pFileToken = "#file";
#endif

		auto GetGLTypeFromStrType = [](const String& _sType)->GLenum
		{
			if (_stricmp(_sType.c_str(), "vertex") == 0)
				return GL_VERTEX_SHADER;
			if (_stricmp(_sType.c_str(), "fragment") == 0)
				return GL_FRAGMENT_SHADER;

			return GL_INVALID_ENUM;
		};

		const char* pTypeToken = "#type ";
		
		String sVertSrc, sFragSrc;
		size_t currentPos = sContent.find(pTypeToken, 0);
		while (currentPos != String::npos)
		{
			size_t EOL = sContent.find("\r\n", currentPos);
			ASSERT(EOL != String::npos, "No shader under type %s", sContent.substr(currentPos).c_str());
			if (EOL == String::npos)
				break;

			String sType = sContent.substr(currentPos + strlen(pTypeToken), EOL - currentPos - strlen(pTypeToken) );
			GLenum eType = GetGLTypeFromStrType(sType);

			ASSERT(eType != GL_INVALID_ENUM, "Unknown shader type");
			ASSERT(m_mapShaderSources.find(eType) == m_mapShaderSources.end(), "Shader type already read");

			size_t nextLinePos = sContent.find_first_not_of("\r\n", EOL);
			currentPos = sContent.find(pTypeToken, nextLinePos);


			// TODO: add #line XX to be able to debug more easyly
			if (currentPos != String::npos)
				m_mapShaderSources[eType] = sContent.substr(nextLinePos, currentPos - nextLinePos);
			else
				m_mapShaderSources[eType] = sContent.substr(nextLinePos);
		}


		Compile();

		m_sName = GetNameFromFile(_sFile);
	}

	void OpenGL_Shader::Load( const String& _sVertSrc, const String& _sFragSrc )
	{
		Clear();
		m_mapShaderSources[GL_VERTEX_SHADER] = _sVertSrc;
		m_mapShaderSources[GL_FRAGMENT_SHADER] = _sFragSrc;
		Compile();
	}

	void OpenGL_Shader::Bind() const
	{
		glUseProgram( m_uProgram );
	}

	void OpenGL_Shader::Unbind() const
	{
		glUseProgram( 0 );
	}

	void OpenGL_Shader::SetUniformInt( const char* _pName, int _int ) const
	{
		GLint iLoc = glGetUniformLocation( m_uProgram, _pName );
		if (iLoc >= 0)
		{
			glUniform1i( iLoc, _int );
		}
	}

	void OpenGL_Shader::SetUniformIntArray( const char* _pName, int* _ints, uint32_t _uCount ) const
	{
		GLint iLoc = glGetUniformLocation( m_uProgram, _pName );
		if (iLoc >= 0)
		{
			glUniform1iv( iLoc, _uCount, _ints );
		}
	}

	void OpenGL_Shader::SetUniformVec4( const char* _pName, const glm::vec4& _vec ) const
	{
		GLint iLoc = glGetUniformLocation( m_uProgram, _pName );
		if (iLoc >= 0)
		{
			glUniform4fv( iLoc, 1, glm::value_ptr( _vec ) );
		}
	}

	void OpenGL_Shader::SetUniformMat4(const char* _pName, const glm::mat4& _mat) const
	{
		GLint iLoc = glGetUniformLocation(m_uProgram, _pName );
		if (iLoc >= 0)
		{
			glUniformMatrix4fv(iLoc, 1, GL_FALSE, glm::value_ptr(_mat));
		}
	}

	void OpenGL_Shader::Clear()
	{
		m_mapShaderSources.clear();
	}

	void OpenGL_Shader::Compile()
	{

		if(m_uProgram == 0)
			m_uProgram = glCreateProgram();

		//////////////////////////////////////////////////////////////////////////
		// SHADERS
		GLint iIsCompiled;

		std::vector<GLuint> vecShaders;

		for (auto& shaderPair : m_mapShaderSources)
		{
			GLenum eShaderType = shaderPair.first;
			const String& sShaderSrc = shaderPair.second;
			GLuint uShader = glCreateShader(eShaderType);

			const GLchar* pSrc = (const GLchar*)sShaderSrc.c_str();
			glShaderSource(uShader, 1, &pSrc, nullptr);
			glCompileShader(uShader);

			glGetShaderiv(uShader, GL_COMPILE_STATUS, &iIsCompiled);
			if (iIsCompiled == GL_FALSE)
			{
				GLint iMaxLength = 0;
				glGetShaderiv(uShader, GL_INFO_LOG_LENGTH, &iMaxLength);

				GLchar* pLog = NEW GLchar[iMaxLength];
				glGetShaderInfoLog(uShader, iMaxLength, &iMaxLength, &pLog[0]);

				Log::Error("Shader failed to compile: {}", pLog);

				glDeleteShader(uShader);
				break;
			}

			glAttachShader(m_uProgram, uShader);
			vecShaders.push_back(uShader);
		}


		//////////////////////////////////////////////////////////////////////////
		// PROGRAM LINK

		glLinkProgram(m_uProgram);

		GLint iIsLinked;
		glGetProgramiv(m_uProgram, GL_LINK_STATUS, &iIsLinked);
		if (iIsLinked == GL_FALSE)
		{
			GLint iMaxLength = 0;
			glGetProgramiv(m_uProgram, GL_INFO_LOG_LENGTH, &iMaxLength);

			GLchar* pLog = NEW GLchar[iMaxLength];
			glGetProgramInfoLog(m_uProgram, iMaxLength, &iMaxLength, &pLog[0]);

			Log::Error("Program did not link: {}", pLog);

			for (GLuint uShader : vecShaders)
				glDeleteShader(uShader);
			glDeleteProgram(m_uProgram);
			return;
		}


		// all went well, free the shaders as we no longer need them
		for (GLuint uShader : vecShaders)
		{
			glDetachShader(m_uProgram, uShader);
			glDeleteShader(uShader);
		}
	}
}
