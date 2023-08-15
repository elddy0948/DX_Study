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