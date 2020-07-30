#include <Plop_pch.h>
#include "OpenGL_VertexArray.h"

#include <GL/glew.h>

namespace Plop
{
	OpenGL_VertexArray::OpenGL_VertexArray()
	{
		glCreateVertexArrays(1, &m_uID);
	}

	OpenGL_VertexArray::~OpenGL_VertexArray()
	{
		glDeleteVertexArrays(1, &m_uID);
	}

	void OpenGL_VertexArray::Bind() const
	{
		glBindVertexArray(m_uID);
	}

	void OpenGL_VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGL_VertexArray::SetIndexBuffer(IndexBuffer& _IndexBuffer)
	{

	}
}