#pragma once

#include <Windows.h>

class Window
{
private:
	class WindowClass
	{
	public:
		static LPCWSTR GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;

		static constexpr LPCWSTR name = L"Joons Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInstance;
	};
public:
	Window(int width, int height, LPCWSTR name) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT CALLBACK HandleMsgSetup(
		HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
	) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(
		HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
	) noexcept;
	LRESULT HandleMsg(
		HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
	) noexcept;
private:
	int width;
	int height;
	HWND hWnd;
};