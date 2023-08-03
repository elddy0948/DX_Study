#pragma once

#include "D3DHelper.h"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

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
	bool InitD3D();
	int Run();

	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	static D3DApp* mApp;

	HINSTANCE mhAppInst = nullptr;
	HWND mhMainWnd = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;

	// Descriptor size
	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvDescriptorSize = 0;

	std::wstring mMainWndCaption = L"First Cube";

	int mClientWidth = 800;
	int mClientHeight = 600;
};