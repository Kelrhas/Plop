#pragma once

namespace Plop
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Create( const String& _sVertSrc, const String& _sFragSrc ) = 0;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

	protected:
	};

	class ShaderLib
	{
		ShaderLib();
	};
}
