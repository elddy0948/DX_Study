#include "BaseApp.h"

BaseApp::BaseApp(HINSTANCE hInstance) : mhInstance(hInstance)
{}

BaseApp::~BaseApp()
{

}

void BaseApp::CreateDevice()
{
#if defined(DEBUG) || defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
	debugController->EnableDebugLayer();
#endif

	ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&m_dxgiFactory)));
	
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_device));

	if (FAILED(hardwareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		));
	}
}

void BaseApp::CreateFenceAndGetDescriptorSize()
{
	ThrowIfFailed(m_device->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_fence)
	));

	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvsrvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void BaseApp::Check4xMSAAQualityLevels()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS multisampleQualityLevels = {};
	multisampleQualityLevels.Format = m_backBufferFormat;
	multisampleQualityLevels.SampleCount = 4;
	multisampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	multisampleQualityLevels.NumQualityLevels = 0;

	ThrowIfFailed(m_device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&multisampleQualityLevels,
		sizeof(multisampleQualityLevels)
	));

	m_4xMSAAQuality = multisampleQualityLevels.NumQualityLevels;
	assert(m_4xMSAAQuality > 0 && "Unexpected MSAA quality level.");
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
		IID_PPV_ARGS(m_commandAllocator.GetAddressOf())
	));

	ThrowIfFailed(m_device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_commandAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(m_commandList.GetAddressOf())
	));

	m_commandList->Close();
}

void BaseApp::CreateSwapChain()
{
	m_swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	swapChainDesc.BufferDesc.Width = m_clientWidth;
	swapChainDesc.BufferDesc.Height = m_clientHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = m_backBufferFormat;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SampleDesc.Count = m_4xMSAAState ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = m_4xMSAAState ? (m_4xMSAAQuality - 1) : 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SwapChainBufferCount;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(m_dxgiFactory->CreateSwapChain(
		m_commandQueue.Get(),
		&swapChainDesc,
		m_swapChain.GetAddressOf()
	));
}

void BaseApp::CreateDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_device->CreateDescriptorHeap(
		&rtvHeapDesc,
		IID_PPV_ARGS(m_rtvHeap.GetAddressOf())
	));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_device->CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(m_dsvHeap.GetAddressOf())
	));
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

D3D12_CPU_DESCRIPTOR_HANDLE BaseApp::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
		m_currentBackBuffer,
		m_rtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE BaseApp::DepthStencilView() const
{
	return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}