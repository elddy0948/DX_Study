#include "BaseApp.h"

BaseApp* BaseApp::m_app = nullptr;

BaseApp::BaseApp(HINSTANCE hInstance) : m_hInstance(hInstance)
{
	assert(m_app == nullptr);
	m_app = this;
}

BaseApp::~BaseApp()
{
	if (m_device != nullptr)
	{
		FlushCommandQueue();
	}
}

BaseApp* BaseApp::GetApp()
{
	return m_app;
}

HINSTANCE BaseApp::AppInstance() const
{
	return m_hInstance;
}

float BaseApp::AspectRatio() const
{
	return static_cast<float>(m_clientWidth) / static_cast<float>(m_clientHeight);
}

bool BaseApp::Get4xMSAAState() const
{
	return m_4xMSAAState;
}

void BaseApp::Set4xMSAAState(bool state)
{
	if (m_4xMSAAState != state) 
	{ 
		m_4xMSAAState = state;
		CreateSwapChain();
		OnResize();
	}
}

bool BaseApp::Initialize()
{
	LoadPipeline();
	OnResize();

	return true;
}

LRESULT BaseApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_appPaused = true;
			Win32Application::GetTimer().Stop();
		}
		else
		{
			m_appPaused = false;
			Win32Application::GetTimer().Start();
		}
		return 0;

	case WM_SIZE:
		m_clientWidth = LOWORD(lParam);
		m_clientHeight = HIWORD(lParam);
		if (m_device)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_appPaused = true;
				m_minimized = true;
				m_maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_appPaused = false;
				m_minimized = false;
				m_maximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (m_minimized)
				{
					m_appPaused = false;
					m_minimized = false;
					OnResize();
				}
				else if (m_maximized)
				{
					m_appPaused = false;
					m_maximized = true;
					OnResize();
				}
				else if (m_resizing)
				{
				}
				else
				{
					OnResize();
				}
			}
		}
		return 0;

	case WM_ENTERSIZEMOVE:
		m_appPaused = true;
		m_resizing = true;
		Win32Application::GetTimer().Stop();
		return 0;
		
	case WM_EXITSIZEMOVE:
		m_appPaused = false;
		m_resizing = false;
		Win32Application::GetTimer().Start();
		OnResize();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return 0;
		}

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
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
	swapChainDesc.OutputWindow = Win32Application::GetHwnd();
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

void BaseApp::OnResize()
{
	assert(m_device);
	assert(m_swapChain);
	assert(m_commandAllocator);

	FlushCommandQueue();

	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	for (int i = 0; i < SwapChainBufferCount; ++i)
		m_swapChainBuffer[i].Reset();
	m_depthStencilBuffer.Reset();

	ThrowIfFailed(m_swapChain->ResizeBuffers(
		SwapChainBufferCount,
		m_clientWidth,
		m_clientHeight,
		m_backBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	));

	m_currentBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; ++i)
	{
		ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_swapChainBuffer[i])));
		m_device->CreateRenderTargetView(m_swapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_rtvDescriptorSize);
	}

	CreateDepthStencilView();

	ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	FlushCommandQueue();

	SetupViewportAndScissorRect();
}

void BaseApp::CreateRenterTargetView()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(
		m_rtvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	for (UINT i = 0; i < SwapChainBufferCount; ++i)
	{
		ThrowIfFailed(m_swapChain->GetBuffer(
			i,
			IID_PPV_ARGS(&m_swapChainBuffer[i])
		));

		m_device->CreateRenderTargetView(
			m_swapChainBuffer[i].Get(),
			nullptr,
			rtvHeapHandle
		);

		rtvHeapHandle.Offset(1, m_rtvDescriptorSize);
	}
}

void BaseApp::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_clientWidth;
	depthStencilDesc.Height = m_clientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = m_depthStencilFormat;
	depthStencilDesc.SampleDesc.Count = m_4xMSAAState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m_4xMSAAState ? (m_4xMSAAQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = m_depthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())));

	m_device->CreateDepthStencilView(
		m_depthStencilBuffer.Get(),
		nullptr,
		DepthStencilView());

	m_commandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			m_depthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void BaseApp::SetupViewportAndScissorRect()
{
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<float>(m_clientWidth);
	m_viewport.Height = static_cast<float>(m_clientHeight);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	//m_commandList->RSSetViewports(1, &m_viewport);

	m_scissorRect = { 0, 0, m_clientWidth / 2, m_clientHeight / 2 };
	//m_commandList->RSSetScissorRects(1, &m_scissorRect);
}

void BaseApp::LoadPipeline()
{
	CreateDevice();
	CreateFenceAndGetDescriptorSize();
	Check4xMSAAQualityLevels();

#ifdef _DEBUG
	LogAdapters();
#endif

	CreateCommandObjects();
	CreateSwapChain();
	CreateDescriptorHeaps();
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

ID3D12Resource* BaseApp::CurrentBackBuffer() const
{
	return m_swapChainBuffer[m_currentBackBuffer].Get();
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