#pragma once

#include <Renderer/Buffers.h>

namespace Plop
{
	//////////////////////////////////////////////////////////////////////////
	// VertexArray
	class VertexArray
	{
	public:
		VertexArray() = default;
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetIndexBuffer(const IndexBufferPtr& _xIndexBuffer) = 0;
		virtual void AddVertexBuffer(const VertexBufferPtr& _xVertexBuffer) = 0;

		virtual void Draw() const = 0;


		static VertexArray* Create();

	protected:
	};

	using VertexArrayPtr = std::shared_ptr<VertexArray>;
}
