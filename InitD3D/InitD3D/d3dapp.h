#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <string>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class D3DApp
{
protected:
	D3DApp(HINSTANCE hInstance);

public:
	static D3DApp* GetApp();
	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int Run();

protected:
	bool InitMainWindow();
	bool InitDirect3D();

protected:
	static D3DApp* mApp;

	HINSTANCE mhAppInstance = nullptr;
	HWND mhMainWindow = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;

	std::wstring mMainWindowCaption = L"d3d app";
	int mClientWidth = 800;
	int mClientHeight = 600;
};