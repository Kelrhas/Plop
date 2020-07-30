#pragma once

#include <Gl/glew.h>

#include <Renderer/Shader.h>

namespace Plop
{
	class OpenGL_Shader : public Shader
	{
	public:
		OpenGL_Shader();
		~OpenGL_Shader();

		virtual void Create( const String& _sVertSrc, const String& _sFragSrc ) override;
		virtual void Bind() override;
		virtual void Unbind() override;
		
	private:
		GLuint m_uVertShader;
		GLuint m_uFragShader;
		GLuint m_uProgram;
	};
}
