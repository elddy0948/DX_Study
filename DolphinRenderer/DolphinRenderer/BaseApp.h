#pragma once
#ifndef BASE_APP_H
#define BASE_APP_H

#include "Headers.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class BaseApp {
public:
	BaseApp();
	~BaseApp();

public:
	void Initialize();
	int Run();

private:
	HWND m_hwnd;
	HINSTANCE m_hInstance;
};

#endif /* !BASE_APP_H */
