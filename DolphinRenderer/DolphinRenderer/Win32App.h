#pragma once
#ifndef WIND32_APP_H
#define WIND32_APP_H

#include "Headers.h"
#include "BaseApp.h"

class BaseApp;

class Win32App {
public:
	static int Run(BaseApp* pApp, HINSTANCE hInstance, int nCmdShow);
	static HWND GetHwnd() { return m_hwnd; }

protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static HWND m_hwnd;
};

#endif /* !WIND32_APP_H */
