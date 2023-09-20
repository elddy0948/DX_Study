#pragma once

#include "Helper.h"
#include "BaseApp.h"
#include "JRMath.h"
#include "Material.h"
#include "UploadBuffer.h"
#include "SubmeshGeometry.h"

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

// Mesh Geometry
struct MeshGeometry
{
	std::string Name;

	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;;
	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;

	DXGI_FORMAT IndexBufferFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;
		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexBufferFormat;
		ibv.SizeInBytes = IndexBufferByteSize;
		return ibv;
	}

	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

// Render Item
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

	void DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems);

private:
	void BuildMaterials();

private:
	FrameResource* m_currentFrameResource = nullptr;

	std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
};