#pragma once

#include "resource.h"
#include "framework.h"
#include "Engine.h"

#ifndef Assert
#if defined(DEBUG) || defined(_DEBUG)
#define Assert(b) do {if (!(b)) { OutputDebugStringA("Assert : " #b "\n");}} while(0)
#else
#define Assert(b)
#endif
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class MainApp
{
public:
	MainApp();
	~MainApp();

	HRESULT Initialize();
	void RunMessageLoop();

private:
	HWND m_hWnd;
	Engine* engine;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};