#pragma once

#include <Renderer/Shader.h>
#include <Renderer/VertexArray.h>
#include <Renderer/Texture.h>

namespace Plop
{
	class Mesh;
	using MeshPtr = std::shared_ptr<Mesh>;

	class Mesh
	{
	public:
		ShaderPtr		m_xShader = nullptr;
		VertexArrayPtr	m_xVertexArray = nullptr;
		glm::mat4		m_mTransform = glm::identity<glm::mat4>();
		TexturePtr		m_xTex = nullptr;
		int				m_iTexSlot = 0;
	};
}
