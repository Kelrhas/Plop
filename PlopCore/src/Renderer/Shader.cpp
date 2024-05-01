#include "Config.h"

#include "Shader.h"

#include "Debug/Debug.h"
#include "Platform/OpenGL/OpenGL_Shader.h"
#include "Renderer/Renderer.h"

#include <filesystem>

namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	// Shader

	String Shader::GetNameFromFile(const StringPath &_sFile)
	{
		return _sFile.filename().string();
	}

	ShaderPtr Shader::Create(const StringPath & _sFile)
	{
		ShaderPtr xShader = nullptr;
		switch (Renderer::GetAPI())
		{
			case RenderAPI::API::OPENGL:		xShader = std::make_shared<OpenGL_Shader>();
		}

		if (xShader)
		{
			xShader->Load(_sFile);
			return xShader;
		}


		ASSERTM(false, "Render API not supported");
		return nullptr;
	}


	//////////////////////////////////////////////////////////////////////////
	// ShaderLibrary

	ShaderPtr ShaderLibrary::Load( const StringPath& _sFile )
	{
		auto it = m_mapShaders.find(Shader::GetNameFromFile(_sFile));
		if (it != m_mapShaders.end())
			return it->second;

		ShaderPtr xShader = Shader::Create( _sFile );
		Add( xShader );
		return xShader;
	}

	void ShaderLibrary::Add( const ShaderPtr& _xShader )
	{
		auto it = m_mapShaders.find( _xShader->GetName() );
		ASSERTM( it == m_mapShaders.end(), "Shader already exists" );
		if (it == m_mapShaders.end())
		{
			m_mapShaders[_xShader->GetName()] = _xShader;
		}
	}

	ShaderPtr ShaderLibrary::GetShader( const String& _sName )
	{
		auto it = m_mapShaders.find( _sName );
		ASSERTM( it != m_mapShaders.end(), "Shader not found" );
		if (it != m_mapShaders.end())
			return it->second;

		return nullptr;
	}
}
