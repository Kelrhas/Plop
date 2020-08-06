#pragma once

#include <Renderer/Shader.h>
#include <Renderer/VertexArray.h>

namespace Plop
{
	class Mesh;
	using MeshPtr = std::shared_ptr<Mesh>;

	class Mesh
	{
	public:
		ShaderPtr		m_xShader;
		VertexArrayPtr	m_xVertexArray;
		glm::mat4		m_mTransform = glm::identity<glm::mat4>();
		int				m_iTexSlot = 0;
	};
}
