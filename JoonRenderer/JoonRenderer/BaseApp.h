#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>

#include "Helper.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class BaseApp
{
public:
	BaseApp(HINSTANCE hInstance);
	BaseApp(const BaseApp& rhs) = delete;
	BaseApp& operator=(const BaseApp& rhs) = delete;
	~BaseApp();

protected:
	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	void CheckFeatureSupport();


	void FlushCommandQueue();

private:
	void CreateDevice();
	void CreateFenceAndGetDescriptorSize();
	void Check4xMSAAQualityLevels();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateDescriptorHeaps();

private:
	static const int SwapChainBufferCount = 2;

	HINSTANCE mhInstance;
	HWND m_hWnd;

	UINT m_currentFence = 0;
	UINT m_rtvDescriptorSize = 0;
	UINT m_dsvDescriptorSize = 0;
	UINT m_cbvsrvDescriptorSize = 0;
	UINT m_4xMSAAQuality = 0;

	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	bool m_4xMSAAState = false;

	int m_currentBackBuffer = 0;
	int m_clientWidth = 800;
	int m_clientHeight = 600;

	Microsoft::WRL::ComPtr<ID3D12Device> m_device = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList = nullptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap = nullptr;
};