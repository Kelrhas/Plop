#pragma once

#include <memory>

namespace Plop
{
	class Shader;
	using ShaderPtr = std::shared_ptr<Shader>;

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Load( const String& _sFile ) = 0;
		virtual void Load( const String& _sVertSrc, const String& _sFragSrc ) = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniformMat4(const String& _sName, const glm::mat4& _mMat) const = 0;


		static ShaderPtr Create(const String& _sFile);
		static ShaderPtr Create(const String& _sVertSrc, const String& _sFragSrc);

	protected:
	};
}
