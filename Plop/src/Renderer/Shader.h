#pragma once

namespace Plop
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Create( const String& _sVertSrc, const String& _sFragSrc ) = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

	protected:
	};

	class ShaderLib
	{
		ShaderLib();
	};
}
