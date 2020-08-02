#pragma once

#include <Renderer/Shader.h>
#include <Renderer/VertexArray.h>

namespace Plop
{
	class Mesh
	{
	public:
		ShaderPtr		m_xShader;
		VertexArrayPtr	m_xVertexArray;
		glm::mat4		m_mTransform = glm::identity<glm::mat4>();
	};
	using MeshPtr = std::shared_ptr<Mesh>;
}
