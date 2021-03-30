#include "Plop_pch.h"
#include "Shader.h"

#include <filesystem>

#include <Platform/OpenGL/OpenGL_Shader.h>
#include <Renderer/Renderer.h>

namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	// Shader

	String Shader::GetNameFromFile(const String& _sFile)
	{
		std::filesystem::path sPath = _sFile;
		return sPath.filename().string();
	}

	ShaderPtr Shader::Create(const String& _sFile)
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

	ShaderPtr ShaderLibrary::Load( const String& _sFile )
	{
		auto& it = m_mapShaders.find(Shader::GetNameFromFile(_sFile));
		if (it != m_mapShaders.end())
			return it->second;

		ShaderPtr xShader = Shader::Create( _sFile );
		Add( xShader );
		return xShader;
	}

	void ShaderLibrary::Add( const ShaderPtr& _xShader )
	{
		auto& it = m_mapShaders.find( _xShader->GetName() );
		ASSERTM( it == m_mapShaders.end(), "Shader already exists" );
		if (it == m_mapShaders.end())
		{
			m_mapShaders[_xShader->GetName()] = _xShader;
		}
	}

	ShaderPtr ShaderLibrary::GetShader( const String& _sName )
	{
		auto& it = m_mapShaders.find( _sName );
		ASSERTM( it != m_mapShaders.end(), "Shader not found" );
		if (it != m_mapShaders.end())
			return it->second;

		return nullptr;
	}
}
