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

		virtual void Load( const String& _sFile ) override;
		virtual void Load( const String& _sVertSrc, const String& _sFragSrc ) override;
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetUniformInt( const char* _pName, int _int ) const override;
		virtual void SetUniformIntArray( const char* _pName, int* _ints, uint32_t _uCount ) const override;
		virtual void SetUniformVec4( const char* _pName, const glm::vec4& _vec ) const override;
		virtual void SetUniformMat4( const char* _pName, const glm::mat4& _mat) const override;
		
	private:
				void Clear();
				void Compile();

		GLuint								m_uProgram = 0;
		std::unordered_map<GLenum, String>	m_mapShaderSources;
	};
}
