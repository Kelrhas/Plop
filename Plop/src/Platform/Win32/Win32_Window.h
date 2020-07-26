#pragma once

#include <Window.h>

namespace Plop
{
	class Win_Window : public Window
	{
	public:
		Win_Window(WindowConfig& _config);
		virtual ~Win_Window() override;

		virtual void		Init() override;
		virtual void		Destroy() override;
		virtual void		Update() override;

		virtual void		ToggleFullscreen() override;
		virtual void		SetVSync(bool _bEnabled) override;

				LRESULT		WindowCallback(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam);

				HWND		GetHWND() const { return m_hWnd; }
	private:




				HWND		m_hWnd;
				HDC			m_hDeviceContext;
				HGLRC		m_hGLRenderContext;
	};
}

/*
class Window{
public:  
	bool CreateWindow();
				   // Remeber to #undef CreateWindow otherwise compiler will spew, or you could name it differentprotected:   
virtual LRESULT HandleMessage(UINT umsg, WPARAM wparam, LPARAM lparam);
private: 
	HWND hwnd; 
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
};

bool Window::CreateWindow()
{
	hwnd = CreateWindowEx(...);
	if (!hwnd)  
		return false; 
	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));  
	// Additional window setup code
	return true;
}

LRESULT HandleMessage(UINT umsg, WPARAM wparam, LPARAM lparam)
{ 
	return DefWindowProc(this->hwnd, umsg, wparam, lparam);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	// Get the window object attached to the window handle 
	Window* wnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)); 
	if (wnd)
		return wnd->HandleMessage(umsg, wparam, lparam);
	else 
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}
/**/