#pragma once

#include "Renderer/RenderContext.h"

#include <Windows.h>

namespace Plop
{
	class OpenGLWin32_Context : public RenderContext
	{
	public:
		OpenGLWin32_Context() : RenderContext() {}

		virtual void Init() override;
		virtual void Flush() override;
		virtual void Destroy() override;

		virtual void SetCurrent() override;
		virtual void SetVSync(bool _bEnabled) override;

		virtual void* GetNativeContext() const override { return m_hGLRenderContext; }

	private:
		HDC			m_hDeviceContext = nullptr;
		HGLRC		m_hGLRenderContext = nullptr;
	};
}

