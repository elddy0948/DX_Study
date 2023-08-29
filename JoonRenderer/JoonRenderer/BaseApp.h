#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi.h>
#include <vector>
#include <string>

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


private:
	HINSTANCE mhInstance;

	Microsoft::WRL::ComPtr<IDXGIFactory> mdxgiFactory = nullptr;
};