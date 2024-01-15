#include "TriangleApp.h"

TriangleApp::TriangleApp(UINT width, UINT height, std::wstring name) : BaseApp(width, height, name), m_frameIndex(0), m_viewport(), m_rtvDescriptorSize(0) {
	m_viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height) };
	m_scissorRect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
}

void TriangleApp::OnInit() {
	LoadPipeline();
	LoadAssets();
}

void TriangleApp::OnUpdate() {

}

void TriangleApp::OnRender() {

}

void TriangleApp::OnDestroy() {

}

void TriangleApp::LoadPipeline() {
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	{
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(debugController), (void**)&debugController))) {
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	IDXGIFactory4* factory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(factory), (void**)&factory));

	if (m_useWarpDevice) {
		IDXGIAdapter* warpAdapter;
		ThrowIfFailed(factory->EnumWarpAdapter(__uuidof(warpAdapter), (void**)&warpAdapter));
		ThrowIfFailed(D3D12CreateDevice(warpAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(m_device), (void**)&m_device));
	}
	else {
		IDXGIAdapter1* hardwareAdapter;
		GetHardwareAdapter(factory, &hardwareAdapter);
		ThrowIfFailed(D3D12CreateDevice(hardwareAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(m_device), (void**)&m_device));
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	
	ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, __uuidof(m_commandQueue), (void**)&m_commandQueue));

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.BufferCount = FrameCount;
	swapchainDesc.Width = m_width;
	swapchainDesc.Height = m_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.SampleDesc.Count = 1;

	IDXGISwapChain1* swapchain;

	ThrowIfFailed(factory->CreateSwapChainForHwnd(m_commandQueue, Win32App::GetHwnd(), &swapchainDesc, nullptr, nullptr, &swapchain));

	ThrowIfFailed(factory->MakeWindowAssociation(Win32App::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

	memcpy_s(m_swapchain, sizeof(m_swapchain), swapchain, sizeof(swapchain));

	m_frameIndex = m_swapchain->GetCurrentBackBufferIndex();

	/* Descriptor heaps */
	{
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(m_rtvHeap), (void**)&m_rtvHeap));
		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	/* Frame Resources */
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		for (UINT n = 0; n < FrameCount; ++n) {
			ThrowIfFailed(m_swapchain->GetBuffer(n, __uuidof(m_renderTargets[n]), (void**)&m_renderTargets[n]));
			m_device->CreateRenderTargetView(m_renderTargets[n], nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

	ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(m_commandAllocator), (void**)&m_commandAllocator));
}


