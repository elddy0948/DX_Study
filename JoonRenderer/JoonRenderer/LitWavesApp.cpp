#include "LitWavesApp.h"

// FrameResource
FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT wavesVertexCount, UINT materialCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocator.GetAddressOf())));
	ObjectConstantsBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
	PassConstantsBuffer = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	VertexBuffer = std::make_unique<UploadBuffer<Vertex>>(device, wavesVertexCount, false);
	MaterialConstantsBuffer = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
}

FrameResource::~FrameResource() {}

// LitWavesApp
LitWavesApp::LitWavesApp(HINSTANCE hInstance) : BaseApp(hInstance) {}

LitWavesApp::~LitWavesApp()
{
	BaseApp::~BaseApp();
}

void LitWavesApp::BuildMaterials()
{
	auto grass = std::make_unique<Material>();
	grass->Name = "grass";
	grass->ConstantBufferIndex = 0;
	grass->DiffuseAlbedo = XMFLOAT4(0.2f, 0.6f, 0.6f, 1.0f);
	grass->Roughness = 0.125f;

	auto water = std::make_unique<Material>();
	water->Name = "water";
	water->ConstantBufferIndex = 1;
	water->DiffuseAlbedo = XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
	water->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	water->Roughness = 0.0f;

	m_materials["grass"] = std::move(grass);
	m_materials["water"] = std::move(water);
}