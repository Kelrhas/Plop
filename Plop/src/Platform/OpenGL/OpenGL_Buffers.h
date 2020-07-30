#pragma once
#include <Renderer/Buffers.h>

namespace Plop
{
	class OpenGL_VertexBuffer : public VertexBuffer
	{
	public:
		OpenGL_VertexBuffer(size_t _uSize);
		OpenGL_VertexBuffer(uint32_t _uCount, float* _pVerts);
		virtual ~OpenGL_VertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetLayout(const BufferLayout& _layout) override;

	private:
		uint32_t m_uID = 0;
	};

	class OpenGL_IndexBuffer : public IndexBuffer
	{
	public:
		OpenGL_IndexBuffer(uint32_t _uCount, uint32_t* _pIndices);
		virtual ~OpenGL_IndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

	private:
		uint32_t m_uID = 0;
	};
}
