#pragma once

#include <Renderer/RenderContext.h>

namespace Plop
{
	class OpenGLWin32_Context : public RenderContext
	{
	public:
		OpenGLWin32_Context(Window& _window) : RenderContext(_window) {}

		virtual void Init() override;
		virtual void Flush() override;
		virtual void Destroy() override;

		virtual void SetVSync(bool _bEnabled) override;

	private:
		HDC			m_hDeviceContext;
		HGLRC		m_hGLRenderContext;
	};
}

