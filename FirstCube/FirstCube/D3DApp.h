#pragma once

#include "D3DHelper.h"
#include "GameTimer.h"

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

	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeap();

	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	void CreateRenderTargetView();
	void CreateDepthStencilBuffer();
	void CreateDepthStencilView();
	void SetViewports();
	void SetScissorRect();
	
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	static D3DApp* mApp;

	HINSTANCE mhAppInst = nullptr;
	HWND mhMainWnd = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	
	// Command objects
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	// Descriptor Heap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	// Descriptor size
	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvDescriptorSize = 0;

	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT m4xMsaaQuality = 0;
	bool m4xMsaaState = false;

	static const int SwapChainBufferCount = 2;
	int mCurrentBackBuffer = 0;

	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];

	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D16_UNORM;

	D3D12_RECT mScissorRect;

	GameTimer mGameTimer;

	std::wstring mMainWndCaption = L"First Cube";

	int mClientWidth = 800;
	int mClientHeight = 600;
};