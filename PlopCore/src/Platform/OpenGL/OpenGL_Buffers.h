#pragma once
#include <Renderer/Buffers.h>

namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	// VertexBuffer
	class OpenGL_VertexBuffer : public VertexBuffer
	{
	public:
		OpenGL_VertexBuffer(size_t _uSize);
		OpenGL_VertexBuffer( size_t _uSize, void* _pData);
		virtual ~OpenGL_VertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		// for now we only support float elements
		virtual void SetData( size_t _uSize, void* _pData ) override;

	private:
		uint32_t m_uID = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	// IndexBuffer
	class OpenGL_IndexBuffer : public IndexBuffer
	{
	public:
		OpenGL_IndexBuffer(uint32_t _uCount);
		OpenGL_IndexBuffer(uint32_t _uCount, uint32_t* _pIndices);
		virtual ~OpenGL_IndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData( uint32_t _uCount, uint32_t* _pIndices ) override;

	private:
		uint32_t m_uID = 0;
	};
}
