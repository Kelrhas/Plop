#pragma once
#include <Renderer/VertexArray.h>

namespace Plop
{
	class OpenGL_VertexArray : public VertexArray
	{
	public:
		OpenGL_VertexArray();
		~OpenGL_VertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetIndexBuffer(IndexBuffer& _IndexBuffer) override;


	private:
		uint32_t m_uID;
	};
}
