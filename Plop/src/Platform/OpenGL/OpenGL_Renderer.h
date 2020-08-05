#pragma once

#include <Renderer/Renderer.h>

namespace Plop
{
	class OpenGL_Renderer : public RenderAPI
	{
	public:
		virtual void Init() override;
		virtual void Clear() override;
		virtual void Resize(uint32_t _uWidth, uint32_t _uHeight) override;

		virtual void DrawIndexed(const VertexArrayPtr& _xVertexArray) override;
	private:
	};
}
