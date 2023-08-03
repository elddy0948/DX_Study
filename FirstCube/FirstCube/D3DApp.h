#pragma once

#include <Windows.h>
#include <string>
#include <cassert>

class D3DApp
{
public:
	D3DApp(HINSTANCE hInstance);
	D3DApp(const D3DApp& rhs) = delete;
	D3DApp& operator=(const D3DApp& rhs) = delete;
	virtual ~D3DApp();

public:
	static D3DApp* GetApp();

	bool InitMainWindow();
	int Run();

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	static D3DApp* mApp;

	HINSTANCE mhAppInst = nullptr;
	HWND mhMainWnd = nullptr;

	std::wstring mMainWndCaption = L"First Cube";

	int mClientWidth = 800;
	int mClientHeight = 600;
};