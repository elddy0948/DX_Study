#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <string>
#include <cassert>

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

	bool InitDevice();
	void InitFence();
	void Check4XMSAA();

protected:
	static D3DApp* mApp;

	HINSTANCE mhAppInstance = nullptr;
	HWND mhMainWindow = nullptr;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvDescriptorSize = 0;

	UINT m4xMsaaQuality = 0;

	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	std::wstring mMainWindowCaption = L"d3d app";
	int mClientWidth = 800;
	int mClientHeight = 600;
};