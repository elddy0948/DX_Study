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
	void CreateDevice();
	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

	void CheckFeatureSupport();

	void CreateCommandObjects();

	void FlushCommandQueue();

private:
	HINSTANCE mhInstance;

	Microsoft::WRL::ComPtr<ID3D12Device> m_device = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory6> m_dxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList = nullptr;

	UINT m_currentFence = 0;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence = nullptr;
};