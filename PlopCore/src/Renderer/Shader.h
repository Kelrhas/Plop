#pragma once

#include "Types.h"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <unordered_map>

namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	// Shader
	class Shader;
	class ShaderLibrary;
	using ShaderPtr = std::shared_ptr<Shader>;

	class Shader
	{
		friend class ShaderLibrary;
	public:
		virtual ~Shader() = default;

		virtual void Load( const StringPath & _sFile ) = 0;
		virtual void Load( const String& _sVertSrc, const String& _sFragSrc ) = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

				const String& GetName() const { return m_sName; }

		virtual void SetUniformInt( const char* _pName, int _int) const = 0;
		virtual void SetUniformIntArray( const char* _pName, int* _ints, uint32_t _uCount ) const = 0;
		virtual void SetUniformVec4( const char* _pName, const glm::vec4& _vec ) const = 0;
		virtual void SetUniformMat4( const char* _pName, const glm::mat4& _mat ) const = 0;
		

		static String GetNameFromFile(const StringPath &_sFile);

	protected:
		static ShaderPtr Create(const StringPath & _sFile);

		String		m_sName;
	};

	//////////////////////////////////////////////////////////////////////////
	// ShaderLibrary
	class ShaderLibrary
	{
	public:

		ShaderPtr Load( const StringPath& _sFile );
		void Add( const ShaderPtr& _xShader );
		ShaderPtr GetShader( const String& _sName );

	private:
		std::unordered_map<String, ShaderPtr> m_mapShaders;
	};
}
