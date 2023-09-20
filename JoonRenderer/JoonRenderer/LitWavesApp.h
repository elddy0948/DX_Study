#pragma once

#include "Helper.h"
#include "BaseApp.h"
#include "JRMath.h"
#include "Material.h"
#include "MeshGeometry.h"
#include "UploadBuffer.h"

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 world;
};

struct PassConstants
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 invView;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT4X4 invProj;
	DirectX::XMFLOAT4X4 viewProj;
	DirectX::XMFLOAT4X4 invViewProj;
	DirectX::XMFLOAT3 eyePos;
	DirectX::XMFLOAT2 renderTargetSize;
	DirectX::XMFLOAT2 invRenderTargetSize;

	float nearZ;
	float farZ;
	float totalTime;
	float deltaTime;
};

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

struct MaterialConstants
{
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;
	DirectX::XMFLOAT4X4 MaterialTransform = JRMath::Identity4x4();
};

// Frame Resource
struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT wavesVertexCount, UINT materialCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CommandAllocator;

	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectConstantsBuffer = nullptr;
	std::unique_ptr<UploadBuffer<PassConstants>> PassConstantsBuffer = nullptr;

	std::unique_ptr<UploadBuffer<Vertex>> VertexBuffer = nullptr;
	std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialConstantsBuffer = nullptr;

	UINT64 fence = 0;
};

struct RenderItem
{
public:
	RenderItem() = default;

	DirectX::XMFLOAT4X4 World = JRMath::Identity4x4();

	int ConstantsBufferChangedFlag = NumFrameResource;
	UINT ObjectConstantsBufferIndex = -1;

	MeshGeometry* Geo = nullptr;
	Material* Material = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

class LitWavesApp : public BaseApp
{
public:
	LitWavesApp(HINSTANCE hInstance);
	LitWavesApp(const LitWavesApp& rhs) = delete;
	LitWavesApp& operator=(const LitWavesApp& rhs) = delete;
	~LitWavesApp();

protected:
	void UpdateMaterialConstantBuffers();

private:
	void BuildMaterials();

private:
	FrameResource* m_currentFrameResource = nullptr;

	std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
};