#pragma once

#include <Renderer/Renderer.h>

namespace Plop
{
	class OpenGL_Renderer : public RenderAPI
	{
	public:
		virtual void Init() override;
		virtual void Clear() override;

		virtual void DrawIndexed(const VertexArrayPtr& _xVertexArray) override;
	private:
	};
}
