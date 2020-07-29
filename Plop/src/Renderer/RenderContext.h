#pragma once

#include <Window.h>

namespace Plop
{
	class RenderContext
	{
	public:
		RenderContext(Window& _window) : m_window(_window) {}
		virtual ~RenderContext() = default;

		virtual void Init() = 0;
		virtual void Flush() = 0;
		virtual void Destroy() = 0;

		virtual void SetVSync(bool _bEnabled) = 0;

	protected:
		Window& m_window;
	};
}
