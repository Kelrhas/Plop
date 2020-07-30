#pragma once

#include <Renderer/Buffers.h>

namespace Plop
{
	class VertexArray
	{
	public:
		VertexArray() = default;
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetIndexBuffer(IndexBuffer& _IndexBuffer) = 0;


		static VertexArray* Create();
	};
}
