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

	void OpenGL_VertexArray::SetIndexBuffer(const IndexBufferPtr& _xIndexBuffer)
	{
		m_xIndexBuffer = _xIndexBuffer;
		glBindVertexArray(m_uID);
		_xIndexBuffer->Bind();
	}

	void OpenGL_VertexArray::AddVertexBuffer(const VertexBufferPtr& _xVertexBuffer)
	{
		auto it = std::find(m_vecVertexBuffers.begin(), m_vecVertexBuffers.end(), _xVertexBuffer);
		if (it == m_vecVertexBuffers.end())
		{
			m_vecVertexBuffers.push_back(_xVertexBuffer);
			glBindVertexArray(m_uID);
			_xVertexBuffer->Bind();

			int iAttrib = 0;
			auto& layout = _xVertexBuffer->GetLayout();
			for (const BufferLayout::Element& elmt : layout)
			{
				glEnableVertexAttribArray(iAttrib);
				glVertexAttribPointer(iAttrib, BufferLayout::GetElementCount(elmt.eType), GL_FLOAT, elmt.bNormalized, layout.GetStride(), (const void*)elmt.uOffset);
				++iAttrib;
			}
		}
	}

	void OpenGL_VertexArray::Draw() const
	{
		glBindVertexArray(m_uID);
		glDrawElements(GL_TRIANGLES, m_xIndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}