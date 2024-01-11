#include "Device.h"

Device::Device() {
	mFactory = nullptr;
	mDevice = nullptr;
	m_CmdObjects = nullptr;
}

Device::~Device() {
	delete m_CmdObjects;

	if (mFence) mFence->Release();
	if (mDevice) mDevice->Release();
	if (mFactory) mFactory->Release();

#if defined(DEBUG) || defined(_DEBUG)
	if (debugController) debugController->Release();
#endif
}

bool Device::initialize() {
#if defined(DEBUG) || defined(_DEBUG)
	EnableDebugLayer();
#endif

	HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, __uuidof(mDevice), (void**)&mDevice);
	if (FAILED(hardwareResult)) { return false; }
	
	CreateFence();
	GetDescriptorSize();
	CheckMultisampleQualityLevels();

	/* Commad objects initialize */
	m_CmdObjects = new CommandObjects(this);
	m_CmdObjects->Initialize();

	return true;
}

void Device::GetDescriptorSize() {
	mRTVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDSVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCBVSRVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	std::cout << "Render Target view descriptor size : " << mRTVDescriptorSize << '\n';
	std::cout << "Depth Stencil view descriptor size : " << mDSVDescriptorSize << '\n';
	std::cout << "CBV SRV descriptor size : " << mCBVSRVDescriptorSize << '\n';
}

void Device::CreateFence() {
	mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(mFence), (void**)&mFence);
}

void Device::CheckMultisampleQualityLevels() {
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msQualityLevels.SampleCount = 4;	// 4x Multi sampling
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;

	mDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels));
	m4xMSAAQuality = msQualityLevels.NumQualityLevels;

	assert(m4xMSAAQuality > 0);

	std::cout << "MSAA Quality : " << m4xMSAAQuality << '\n';
}

#if defined(DEBUG) || defined(_DEBUG)
void Device::EnableDebugLayer() {
	HRESULT hr = D3D12GetDebugInterface(__uuidof(debugController), (void**)&debugController);
	assert(hr == S_OK);
	debugController->EnableDebugLayer();
}
#endif