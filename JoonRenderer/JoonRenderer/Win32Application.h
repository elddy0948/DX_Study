#pragma once

#include "BaseApp.h"
#include "GameTimer.h"

class BaseApp;

class Win32Application
{
public:
	static int Run(BaseApp* pApp);
	static HWND GetHwnd() { return m_hwnd; }
	static GameTimer GetTimer() { return m_timer; }
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static void CalculateFrameStats();
private:
	static HWND m_hwnd;
	static GameTimer m_timer;
};