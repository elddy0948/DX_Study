#pragma once
#ifndef BASE_APP_H
#define BASE_APP_H

/* For debugging */
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "GameTimer.h"
#include "Headers.h"

/* d3d12 libraries */
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace Microsoft::WRL;
using namespace std;

class BaseApp {
public:
	BaseApp(HINSTANCE hInstance);
	BaseApp(const BaseApp& rhs) = delete;
	BaseApp& operator=(const BaseApp& rhs) = delete;
	virtual ~BaseApp();

public:
	static BaseApp* GetApp() { return m_app; }
	
	HINSTANCE AppInstance() const { return m_hInstance; }
	HWND MainWindow() const { return m_mainWnd; }
	float AspectRatio() const { return static_cast<float>(m_clientWidth) / static_cast<float>(m_clientHeight); }

	bool Get4xMsaaState() const { return m_4xMsaaState; }
	void Set4xMsaaState(bool value);

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void CreateRTVAndDSVDescriptorHeaps();
	virtual void OnResize();
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}


protected:
	bool InitMainWindow();
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapchain();
	void FlushCommandQueue();

	ID3D12Resource* CurrentBackBuffer() const { return m_swapchainBuffer[m_currentBackBuffer].Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_currentBackBuffer, m_rtvDescriptorSize); }
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const { return m_dsvHeap->GetCPUDescriptorHandleForHeapStart(); }

	void CalculateFrameStats();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

protected:
	static BaseApp* m_app;

	HINSTANCE m_hInstance = nullptr;
	HWND m_mainWnd = nullptr;

	/* App screen states */
	bool m_appPaused = false;
	bool m_minimized = false;
	bool m_maximized = false;
	bool m_resizing = false;
	bool m_fullScreenState = false;

	bool m_4xMsaaState = false;
	UINT m_4xMsaaQuality = 0;

	GameTimer m_timer;

	ComPtr<IDXGIFactory4> m_dxgiFactory;
	ComPtr<IDXGISwapChain> m_swapchain;
	ComPtr<ID3D12Device> m_device;
	ComPtr<ID3D12Fence> m_fence;
	UINT64 m_currentFence = 0;

	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;

	static const int swapchainBufferCount = 2;
	int m_currentBackBuffer = 0;
	ComPtr<ID3D12Resource> m_swapchainBuffer[swapchainBufferCount];
	ComPtr<ID3D12Resource> m_depthStencilBuffer;

	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

	D3D12_VIEWPORT m_screenViewport;
	D3D12_RECT m_scissorRect;

	UINT m_rtvDescriptorSize = 0;
	UINT m_dsvDescriptorSize = 0;
	UINT m_cbvSrvUavDescriptorSize = 0;

	std::wstring m_mainWindowCaption = L"Base App";
	D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_backbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	int m_clientWidth = 800;
	int m_clientHeight = 600;
};

#endif /* !BASE_APP_H */
