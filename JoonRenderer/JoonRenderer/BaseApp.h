#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi.h>
#include <vector>

#include "Helper.h"

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

private:
	HINSTANCE mhInstance;

	Microsoft::WRL::ComPtr<IDXGIFactory> m_dxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
};