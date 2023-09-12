#pragma once

#include "Helper.h"
#include "Win32Application.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

class BaseApp
{
protected:
	BaseApp(HINSTANCE hInstance);
	BaseApp(const BaseApp& rhs) = delete;
	BaseApp& operator=(const BaseApp& rhs) = delete;
	virtual ~BaseApp();

public:
	static BaseApp* GetApp();

	HINSTANCE AppInstance() const;
	float AspectRatio() const;

	bool Get4xMSAAState() const;
	void Set4xMSAAState(bool state);

	int GetWidth() const { return m_clientWidth; }
	int GetHeight() const { return m_clientHeight; }
	std::wstring GetTitle() const { return m_mainWndCaption; }
	bool IsPaused() const { return m_appPaused; }

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual void Update() = 0;
	virtual void Draw() = 0;

protected:
	virtual void CreateDescriptorHeaps();
	virtual void OnResize();

	virtual void OnLeftKeyDown() {}
	virtual void OnRightKeyDown() {}
	virtual void OnUpKeyDown() {}
	virtual void OnDownKeyDown() {}

protected:
	void LoadPipeline();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	void CheckFeatureSupport();
	void FlushCommandQueue();

	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

private:
	void CreateDevice();
	void CreateFenceAndGetDescriptorSize();
	void Check4xMSAAQualityLevels();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRenterTargetView();
	void CreateDepthStencilView();
	void SetupViewportAndScissorRect();

protected:
	static BaseApp* m_app;
	static const int SwapChainBufferCount = 2;

	HINSTANCE m_hInstance = nullptr;

	UINT64 m_currentFence = 0;
	UINT m_rtvDescriptorSize = 0;
	UINT m_dsvDescriptorSize = 0;
	UINT m_cbvsrvDescriptorSize = 0;
	UINT m_4xMSAAQuality = 0;

	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	D3D_DRIVER_TYPE m_driverType = D3D_DRIVER_TYPE_HARDWARE;

	D3D12_VIEWPORT m_viewport = {};
	RECT m_scissorRect = {};

	bool m_4xMSAAState = false;
	bool m_appPaused = false;
	bool m_minimized = false;
	bool m_maximized = false;
	bool m_resizing = false;
	bool m_fullscreenState = false;

	int m_currentBackBuffer = 0;
	int m_clientWidth = 800;
	int m_clientHeight = 600;

	std::wstring m_mainWndCaption = L"Joon Renderer";

	Microsoft::WRL::ComPtr<ID3D12Device> m_device = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_swapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer = nullptr;
};