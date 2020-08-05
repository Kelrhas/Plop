#pragma once

#include <Window.h>

namespace Plop
{
	class Win32_Window : public Window
	{
	public:
		Win32_Window(WindowConfig& _config);
		virtual ~Win32_Window() override;

		virtual void		Init() override;
		virtual void		Destroy() override;
		virtual void		Update(const TimeStep& _timeStep) override;

		virtual void		ToggleFullscreen() override;
		virtual void		SetVSync(bool _bEnabled) override;

		virtual void*		GetNativeWindow() const override { return m_hWnd; }

				LRESULT		WindowCallback(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam);

				HWND		GetHWND() const { return m_hWnd; }


	private:
				HWND		m_hWnd;
	};
}
