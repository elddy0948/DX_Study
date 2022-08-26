#include "Window.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	: hInstance(GetModuleHandle(NULL))
{
	WNDCLASSEX wc = { 0 };

	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszClassName = GetName();
	wc.lpszMenuName = NULL;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(GetName(), GetInstance());
}

LPCWSTR Window::WindowClass::GetName() noexcept
{
	return name;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInstance;
}

Window::Window(int width, int height, LPCWSTR name) noexcept
	: 
	width(width),
	height(height)
{
	RECT windowRect;

	windowRect.left = 100;
	windowRect.right = width + windowRect.left;
	windowRect.top = 100;
	windowRect.bottom = height + windowRect.top;

	AdjustWindowRect(
		&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE
	);

	hWnd = CreateWindow(
		WindowClass::GetName(), L"Joons Engine Window",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL, NULL,
		WindowClass::GetInstance(),
		this
	);

	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

LRESULT WINAPI Window::HandleMsgSetup(
	HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
) noexcept
{
	if (uMsg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

		SetWindowLongPtr(
			hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd)
		);

		SetWindowLongPtr(
			hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk)
		);

		return pWnd->HandleMsg(hWnd, uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(
	HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>(
		GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, uMsg, wParam, lParam);
}
LRESULT Window::HandleMsg(
	HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
) noexcept
{
	switch (uMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}