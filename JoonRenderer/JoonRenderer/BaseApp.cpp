#include "BaseApp.h"

BaseApp::BaseApp(HINSTANCE hInstance) : mhInstance(hInstance)
{}

BaseApp::~BaseApp()
{

}

void BaseApp::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	
	while (m_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		adapter->GetDesc(&adapterDesc);

		std::wstring text = L"*** Adapter: ";
		text += adapterDesc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());
		adapterList.push_back(adapter);
		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);

		if (adapterList[i])
		{
			adapterList[i]->Release();
			adapterList[i] = 0;
		}
	}
}

void BaseApp::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC outputDesc;
		output->GetDesc(&outputDesc);

		std::wstring text = L"*** Output: ";
		text += outputDesc.DeviceName;
		text += L"\n";

		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, DXGI_FORMAT_B8G8R8A8_UNORM);

		if (output)
		{
			output->Release();
			output = 0;
		}

		++i;
	}
}

void BaseApp::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& mode : modeList)
	{
		UINT n = mode.RefreshRate.Numerator;
		UINT d = mode.RefreshRate.Denominator;

		std::wstring text =
			L"Width = " + std::to_wstring(mode.Width) + L" " +
			L"Height = " + std::to_wstring(mode.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) + L"\n";

		::OutputDebugString(text.c_str());
	}
}

void BaseApp::CheckFeatureSupport()
{
	D3D_FEATURE_LEVEL featureLevels[2] =
	{
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsInfo;

	featureLevelsInfo.NumFeatureLevels = 2;
	featureLevelsInfo.pFeatureLevelsRequested = featureLevels;

	ThrowIfFailed(m_device->CheckFeatureSupport(
		D3D12_FEATURE_FEATURE_LEVELS,
		&featureLevelsInfo,
		sizeof(featureLevels)
	));
}

void BaseApp::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(m_device->CreateCommandQueue(
		&queueDesc,
		IID_PPV_ARGS(&m_commandQueue)
	));

	ThrowIfFailed(m_device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_commandAllocator)
	));

	ThrowIfFailed(m_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(&m_commandList)
	));
}

void BaseApp::FlushCommandQueue()
{
	m_currentFence++;

	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_currentFence));

	if (m_fence->GetCompletedValue() < m_currentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_currentFence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}