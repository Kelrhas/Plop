#include "Plop_pch.h"
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

	OpenGL_VertexBuffer::OpenGL_VertexBuffer(size_t _uSize, void* _pData) : VertexBuffer( _uSize, _pData )
	{
		glCreateBuffers(1, &m_uID);
		glBindBuffer(GL_ARRAY_BUFFER, m_uID);
		glBufferData(GL_ARRAY_BUFFER, _uSize, _pData, GL_STATIC_DRAW);
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

	void OpenGL_VertexBuffer::SetData( size_t _uSize, void* _pData )
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_uID );
		glBufferData( GL_ARRAY_BUFFER, _uSize, _pData, GL_DYNAMIC_DRAW );
	}


	//////////////////////////////////////////////////////////////////////////
	// IndexBuffer

	OpenGL_IndexBuffer::OpenGL_IndexBuffer(uint32_t _uCount) : IndexBuffer(_uCount)
	{
		glCreateBuffers(1, &m_uID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _uCount * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
	}

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

	void OpenGL_IndexBuffer::SetData( uint32_t _uCount, uint32_t* _pIndices )
	{
		m_uCount = _uCount;
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_uID );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, _uCount * sizeof( uint32_t ), _pIndices, GL_DYNAMIC_DRAW );
	}

}
