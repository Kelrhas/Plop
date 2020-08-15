#pragma once

#include <Renderer/RenderContext.h>
#include <Constants.h>

#include <TimeStep.h>

namespace Plop
{
	struct WindowConfig
	{
		uint32_t	uWidth = 1600;		// this is the client (viewport) size, not really the window size
		uint32_t	uHeight = 900;
		uint32_t	uPosX = 0;			// this is the client position
		uint32_t	uPosY = 50;
		bool		bFullscreen = false;
	};

	class Window
	{
	public:
		Window(WindowConfig& _config)
			: m_config(_config)
		{
		}

		virtual ~Window() {}

		virtual void		Init() = 0;
		virtual void		Destroy() = 0;
		virtual void		Update(const TimeStep& _timeStep) = 0;

		virtual void		SetVSync(bool _bEnabled) = 0;
		virtual void		ToggleFullscreen() = 0;

		virtual void*		GetNativeWindow() const = 0;

		glm::vec2			GetViewportSize() const { return glm::vec2(m_config.uWidth, m_config.uHeight); }
		unsigned int		GetWidth() const { return m_config.uWidth; }
		unsigned int		GetHeight() const { return m_config.uHeight; }
		float				GetAspectRatio() const { return (float)m_config.uWidth / (float)m_config.uHeight; }
		RenderContextPtr	GetRenderContext() const { return m_xRenderContext; }

		static Window*		Create(WindowConfig& _config);

	protected:
		WindowConfig& m_config;
		RenderContextPtr m_xRenderContext = nullptr;
	};
}

