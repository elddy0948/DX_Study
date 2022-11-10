#include "d3dapp.h"

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return D3DApp::GetApp()->MsgProc(hWnd, msg, wParam, lParam);
}

D3DApp* D3DApp::mApp = nullptr;
D3DApp* D3DApp::GetApp()
{
	return mApp;
}

D3DApp::D3DApp(HINSTANCE hInstance)
	:mhAppInstance(hInstance)
{
	mApp = this;
}

bool D3DApp::Initialize()
{
	if (!InitMainWindow()) { return false; }
	if (!InitDirect3D()) { return false; }
	return true;
}

LRESULT D3DApp::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) { DestroyWindow(mhMainWindow); }
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int D3DApp::Run()
{
	MSG msg = { 0 };
	
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{

		}
	}
	return (int)msg.wParam;
}

bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return false;
	}

	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);

	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWindow = CreateWindow(
		L"MainWnd",
		mMainWindowCaption.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		0,
		0,
		mhAppInstance,
		0
	);

	if (!mhMainWindow)
	{
		MessageBox(0, L"CreateWindow Failed", 0, 0);
		return false;
	}

	ShowWindow(mhMainWindow, SW_SHOW);
	UpdateWindow(mhMainWindow);

	return true;
}

bool D3DApp::InitDirect3D()
{
	CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory));

	if (!InitDevice()) { return false; }
	InitFence();
	Check4XMSAA();
	CreateCommandObjects();

	return true;
}

bool D3DApp::InitDevice()
{
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&md3dDevice)
	);

	if (FAILED(hardwareResult))
	{
		MessageBox(0, L"Failed init", 0, 0);
		return false;
	}

	return true;
}

void D3DApp::InitFence()
{
	md3dDevice->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&mFence)
	);

	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void D3DApp::Check4XMSAA()
{
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;

	md3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels));

	m4xMsaaQuality = msQualityLevels.NumQualityLevels;

	assert(m4xMsaaQuality > 0);
}

void D3DApp::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	md3dDevice->CreateCommandQueue(
		&queueDesc,
		IID_PPV_ARGS(&mCommandQueue));

	md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&mDirectCmdListAlloc));

	md3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(),
		nullptr,
		IID_PPV_ARGS(&mCommandList));

	mCommandList->Close();
}