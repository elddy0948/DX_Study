#pragma once

#include "D3DHelper.h"
#include "GameTimer.h"

using namespace Microsoft::WRL;

class BaseApp
{
public:
	BaseApp(HINSTANCE hInstance);
	BaseApp(const BaseApp& rhs) = delete;
	BaseApp& operator=(const BaseApp& rhs) = delete;
	virtual ~BaseApp();

public:
	static BaseApp* GetApp();

	HINSTANCE AppInstance() const;
	HWND MainWindow() const;
	float AspectRatio() const;

	bool Get4xMsaaState() const;
	void Set4xMsaaState(bool value);

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void CreateRTVAndDSVDescriptorHeap();
	virtual void OnResize();
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;

protected:
	bool InitMainWindow();
	bool InitD3D();
	void CreateCommandObjects();
	void CreateSwapChain();

	void FlushCommandQueue();

	void CalculateFrameStats();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void CreateDepthStencilBuffer();

	ID3D12Resource* CurrentBackBuffer() const
	{
		return mSwapChainBuffer[mCurrentBackBuffer].Get();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			mRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(),
			mCurrentBackBuffer,
			mRTVDescriptorSize
		);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const
	{
		return mDepthStencilViewHeap->GetCPUDescriptorHandleForHeapStart();
	}


private:
	static BaseApp* mApp;

	bool mAppPaused = false;
	bool mResizing = false;

	HINSTANCE mHInstance = nullptr;
	HWND mHMainWindow = nullptr;

	UINT64 mCurrentFence = 0;

	ComPtr<IDXGIFactory4> mDXGIFactory;
	ComPtr<ID3D12Device> mDevice;
	ComPtr<ID3D12Fence> mFence;

	ComPtr<ID3D12CommandQueue> mCommandQueue;
	ComPtr<ID3D12CommandAllocator> mDirectCommandListAllocator;
	ComPtr<ID3D12GraphicsCommandList> mCommandList;

	ComPtr<ID3D12DescriptorHeap> mRenderTargetViewHeap;
	ComPtr<ID3D12DescriptorHeap> mDepthStencilViewHeap;

	UINT mRTVDescriptorSize = 0;
	UINT mDSVDescriptorSize = 0;
	UINT mCBVSRVDescriptorSize = 0;

	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	UINT m4xMsaaQuality = 0;
	bool m4xMsaaState = false;

	static const int SwapChainBufferCount = 2;
	
	int mCurrentBackBuffer = 0;

	ComPtr<IDXGISwapChain> mSwapChain;
	ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	ComPtr<ID3D12Resource> mDepthStencilBuffer;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D16_UNORM;

	D3D12_VIEWPORT mScreenViewport = {};
	D3D12_RECT mScissorRect = {};

	GameTimer mGameTimer = {};

	std::wstring mMainWindowCaption = L"Not Only Cube";

	int mClientWidth = 800;
	int mClientHeight = 600;
};
