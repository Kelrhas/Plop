#pragma once

#include <Plop_pch.h>

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
		virtual void		Update() = 0;

		virtual void		SetVSync(bool _bEnabled) = 0;
		virtual void		ToggleFullscreen() = 0;

		unsigned int		GetWidth() const { return m_config.uWidth; }
		unsigned int		GetHeight() const { return m_config.uHeight; }


		static Window*		Create(WindowConfig& _config);

	protected:
		WindowConfig& m_config;
	};
}

