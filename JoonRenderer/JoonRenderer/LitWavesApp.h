#pragma once

#include "Helper.h"
#include "BaseApp.h"
#include "JRMath.h"
#include "Material.h"
#include "UploadBuffer.h"
#include "Waves.h"
#include "GeometryGenerator.h"

#include "LitMeshGeometry.h"

#define MaxLights 16

struct Light
{
	DirectX::XMFLOAT3 Strength;
	float FalloffStart;
	DirectX::XMFLOAT3 Direction;
	float FalloffEnd;
	DirectX::XMFLOAT3 Position;
	float SpotPower;
};

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

	DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	Light Lights[MaxLights];
};

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
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

// Render Item
struct RenderItem
{
public:
	RenderItem() = default;

	DirectX::XMFLOAT4X4 World = JRMath::Identity4x4();

	int ConstantsBufferChangedFlag = NumFrameResource;
	UINT ObjectConstantsBufferIndex = -1;

	LitMeshGeometry* Geo = nullptr;
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

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update() override;
	virtual void Draw() override;

	void UpdateObjectConstantBuffer();
	void UpdateMainPassConstantBuffer();
	void UpdateMaterialConstantBuffers();
	void UpdateWaves();
	
	void BuildFrameResource();
	void BuildMaterials();
	void BuildRenderItems();

	void BuildRootSignature();
	void BuildShadersAndInputLayout();

	void BuildLandGeometry();
	void BuildWavesGeometry();

	void DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems);

	void BuildPSOs();

private:
	DirectX::XMFLOAT3 GetHillsNormal(float x, float z) const;
	float GetHeight(float x, float z) const;
	int Rand(int a, int b) const;
	float RandF() const;
	float RandF(float a, float b) const;

private:
	std::vector<std::unique_ptr<FrameResource>> m_frameResources;
	FrameResource* m_currentFrameResource = nullptr;
	int m_currentFrameResourceIndex = 0;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

	PassConstants m_mainPassCB;

	std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;
	std::vector<RenderItem*> m_opaqueRenderItems;
	RenderItem* m_wavesRenderItem = nullptr;

	std::unordered_map<std::string, std::unique_ptr<LitMeshGeometry>> m_geometries;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> m_shaders;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_psos;
	std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;

	bool m_isWireFrame = false;

	std::unique_ptr<Waves> m_waves;

	DirectX::XMFLOAT4X4 m_view = JRMath::Identity4x4();
	DirectX::XMFLOAT4X4 m_proj = JRMath::Identity4x4();
	DirectX::XMFLOAT3 m_eyePosition = { 0.0f, 0.0f, 0.0f };

	float m_theta = 1.5f * DirectX::XM_PI;
	float m_phi = DirectX::XM_PIDIV4;
	float m_radius = 5.0f;
};