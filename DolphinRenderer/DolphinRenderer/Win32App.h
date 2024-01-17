#pragma once
#ifndef WIND32_APP_H
#define WIND32_APP_H

#include "Headers.h"
#include "BaseApp.h"
#include "GameTimer.h"

class BaseApp;

class Win32App {
public:
	static int Run(BaseApp* pApp, HINSTANCE hInstance, int nCmdShow);
	static HWND GetHwnd() { return m_hwnd; }
	static void CalculateFrameStats();

protected:
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	static HWND m_hwnd;
	static GameTimer m_timer;
	static bool m_appPaused;
	static std::wstring m_appTitle;
};

#endif /* !WIND32_APP_H */
