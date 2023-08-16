#include "BaseApp.h"

using namespace Microsoft::WRL;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return BaseApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

BaseApp* BaseApp::mApp = nullptr;

BaseApp* BaseApp::GetApp()
{
	return mApp;
}

BaseApp::BaseApp(HINSTANCE hInstance)
	: mHInstance(hInstance)
{
	assert(mApp == nullptr);
	mApp = this;
}

BaseApp::~BaseApp()
{
	if (mDevice != nullptr)
	{
		//FlushCommandQueue();
	}
}

HINSTANCE BaseApp::AppInstance() const
{
	return mHInstance;
}

HWND BaseApp::MainWindow() const
{
	return mHMainWindow;
}

float BaseApp::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

bool BaseApp::Get4xMsaaState() const
{
	return m4xMsaaState;
}
void BaseApp::Set4xMsaaState(bool value)
{
	if (m4xMsaaState != value)
	{
		m4xMsaaState = value;
		CreateSwapChain();
		OnResize();
	}
}

int BaseApp::Run()
{
	MSG msg = { 0 };

	mGameTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mGameTimer.Tick();
			if (!mAppPaused)
			{
				CalculateFrameStats();
				Update(mGameTimer);
				Draw(mGameTimer);
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool BaseApp::InitMainWindow()
{
	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mHInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"Not Only Cube";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT rect = { 0, 0, mClientWidth, mClientHeight };

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	int actualWidth = rect.right - rect.left;
	int actualHeight = rect.bottom - rect.top;

	mHMainWindow = CreateWindow(
		L"Not only cube",
		mMainWindowCaption.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		actualWidth,
		actualHeight,
		0, 0, mHInstance, 0
	);

	if (!mHMainWindow)
	{
		MessageBox(0, L"Create window failed.", 0, 0);
		return false;
	}

	ShowWindow(mHMainWindow, SW_SHOW);
	UpdateWindow(mHMainWindow);

	return true;
}

bool BaseApp::InitD3D()
{
# if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mDXGIFactory)));

	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&mDevice)
	);

	if (FAILED(hardwareResult))
	{
		MessageBox(nullptr, L"Failed to create d3d device.", 0, 0);
		return false;
	}

	ThrowIfFailed(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

	mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDSVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCBVSRVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS multiSampleQualityLevels = {};
	multiSampleQualityLevels.Format = mBackBufferFormat;
	multiSampleQualityLevels.SampleCount = 4;
	multiSampleQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	multiSampleQualityLevels.NumQualityLevels = 0;

	ThrowIfFailed(mDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&multiSampleQualityLevels,
		sizeof(multiSampleQualityLevels)
	));

	m4xMsaaQuality = multiSampleQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA Quality Level");

	CreateCommandObjects();
	CreateSwapChain();
	CreateRTVAndDSVDescriptorHeap();

	return true;
}

void BaseApp::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC qDesc = {};

	qDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	qDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	ThrowIfFailed(mDevice->CreateCommandQueue(
		&qDesc, 
		IID_PPV_ARGS(&mCommandQueue)
	));

	ThrowIfFailed(mDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCommandListAllocator.GetAddressOf())
	));

	ThrowIfFailed(mDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCommandListAllocator.Get(),
		nullptr,
		IID_PPV_ARGS(mCommandList.GetAddressOf())
	));

	mCommandList->Close();
}

void BaseApp::CreateSwapChain()
{
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd = {};

	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = mHMainWindow;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(mDXGIFactory->CreateSwapChain(
		mCommandQueue.Get(),
		&sd,
		mSwapChain.GetAddressOf()
	));
}

void BaseApp::FlushCommandQueue()
{
	mCurrentFence++;
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(
			mCurrentFence,
			eventHandle
		));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void BaseApp::CalculateFrameStats()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;
	frameCount++;

	if ((mGameTimer.GameTime() - timeElapsed) >= 1.0f)
	{
		using namespace std;

		float fps = (float)frameCount;
		float mspf = 1000.0f / fps;

		wstring fpsStr = std::to_wstring(fps);
		wstring mspfStr = std::to_wstring(mspf);

		wstring windowText = mMainWindowCaption + L" fps: " + fpsStr + L" mspf: " + mspfStr;

		SetWindowText(mHMainWindow, windowText.c_str());

		frameCount = 0;
		timeElapsed += 1.0f;
	}
}

void BaseApp::CreateRenderTargetView()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT i = 0; i <SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(
			i,
			IID_PPV_ARGS(&mSwapChainBuffer[i])
		));

		mDevice->CreateRenderTargetView(
			mSwapChainBuffer[i].Get(),
			nullptr,
			rtvHeapHandle
		);

		rtvHeapHandle.Offset(1, mRTVDescriptorSize);
	}
}

void BaseApp::CreateDepthStencilView()
{
	mDevice->CreateDepthStencilView(
		mDepthStencilBuffer.Get(),
		nullptr,
		DepthStencilView()
	);

	mCommandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			mDepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_DEPTH_WRITE
		)
	);
}

void BaseApp::CreateDepthStencilBuffer()
{
	D3D12_RESOURCE_DESC dsDesc = {};

	dsDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	dsDesc.Alignment = 0;
	dsDesc.Width = mClientWidth;
	dsDesc.Height = mClientHeight;
	dsDesc.DepthOrArraySize = 1;
	dsDesc.MipLevels = 1;
	dsDesc.Format = mDepthStencilFormat;
	dsDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	dsDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	dsDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	dsDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear = {};

	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	ThrowIfFailed(mDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&dsDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())
	));
}

bool BaseApp::Initialize()
{
	if (!InitMainWindow()) return false;
	if (!InitD3D()) return false;
	OnResize();
	return true;
}

LRESULT BaseApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mGameTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mGameTimer.Start();
		}
		return 0;
	}
	case WM_ENTERSIZEMOVE:
	{
		mAppPaused = true;
		mResizing = true;
		mGameTimer.Stop();
		return 0;
	}
	case WM_EXITSIZEMOVE:
	{
		mAppPaused = false;
		mResizing = false;
		mGameTimer.Start();
		OnResize();
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_QUIT:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void BaseApp::CreateRTVAndDSVDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};

	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;

	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		&rtvHeapDesc,
		IID_PPV_ARGS(mRenderTargetViewHeap.GetAddressOf())
	));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};

	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;

	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		&dsvHeapDesc,
		IID_PPV_ARGS(mDepthStencilViewHeap.GetAddressOf())
	));
}

void BaseApp::OnResize()
{
	assert(mDevice);
	assert(mSwapChain);
	assert(mDirectCommandListAllocator);

	FlushCommandQueue();

	ThrowIfFailed(mCommandList->Reset(
		mDirectCommandListAllocator.Get(),
		nullptr
	));

	for (int i = 0; i < SwapChainBufferCount; ++i)
	{
		mSwapChainBuffer[i].Reset();
	}
	mDepthStencilBuffer.Reset();

	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		mClientWidth,
		mClientHeight,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	));
	mCurrentBackBuffer = 0;

	CreateRenderTargetView();
	CreateDepthStencilView();
	CreateDepthStencilBuffer();

	ThrowIfFailed(mCommandList->Close());

	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(
		_countof(cmdLists),
		cmdLists
	);

	FlushCommandQueue();

	mScreenViewport.TopLeftX = 0.0f;
	mScreenViewport.TopLeftY = 0.0f;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, mClientWidth, mClientHeight };
}