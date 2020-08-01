#pragma once
#include <Renderer/VertexArray.h>

namespace Plop
{
	class OpenGL_VertexArray : public VertexArray
	{
	public:
		OpenGL_VertexArray();
		virtual ~OpenGL_VertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetIndexBuffer(const IndexBufferPtr& _xIndexBuffer) override;
		virtual void AddVertexBuffer(const VertexBufferPtr& _xVertexBuffer) override;

		virtual void Draw() const override;


	private:
		uint32_t m_uID;

		IndexBufferPtr m_xIndexBuffer;
		std::vector<VertexBufferPtr> m_vecVertexBuffers;
	};
}
