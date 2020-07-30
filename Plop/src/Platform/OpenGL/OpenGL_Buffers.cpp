#include <Plop_pch.h>
#include "OpenGL_Buffers.h"

#include <GL/glew.h>

namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	// VertexBuffer

	OpenGL_VertexBuffer::OpenGL_VertexBuffer(size_t _uSize) : VertexBuffer(_uSize)
	{
		glCreateBuffers(1, &m_uID);
		glBindBuffer(GL_ARRAY_BUFFER, m_uID);
		glBufferData(GL_ARRAY_BUFFER, _uSize, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGL_VertexBuffer::OpenGL_VertexBuffer(uint32_t _uCount, float* _pVerts) : VertexBuffer(_uCount, _pVerts)
	{
		glCreateBuffers(1, &m_uID);
		glBindBuffer(GL_ARRAY_BUFFER, m_uID);
		glBufferData(GL_ARRAY_BUFFER, _uCount * sizeof(float), _pVerts, GL_STATIC_DRAW);
	}

	OpenGL_VertexBuffer::~OpenGL_VertexBuffer()
	{
		glDeleteBuffers(1, &m_uID);
	}

	void OpenGL_VertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_uID);
	}

	void OpenGL_VertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGL_VertexBuffer::SetLayout(const BufferLayout& _layout)
	{
		int iAttrib = 0;
		for (const BufferLayout::Element& elmt : _layout)
		{
			glEnableVertexAttribArray(iAttrib);
			glVertexAttribPointer(iAttrib, BufferLayout::GetElementCount(elmt.eType), GL_FLOAT, elmt.bNormalized, _layout.GetStride(), (const void*)elmt.uOffset);
			++iAttrib;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// IndexBuffer

	OpenGL_IndexBuffer::OpenGL_IndexBuffer(uint32_t _uCount, uint32_t* _pIndices) : IndexBuffer(_uCount, _pIndices)
	{
		glCreateBuffers(1, &m_uID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _uCount * sizeof(uint32_t), _pIndices, GL_STATIC_DRAW);
	}

	OpenGL_IndexBuffer::~OpenGL_IndexBuffer()
	{
		glDeleteBuffers(1, &m_uID);
	}

	void OpenGL_IndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uID);
	}

	void OpenGL_IndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}
