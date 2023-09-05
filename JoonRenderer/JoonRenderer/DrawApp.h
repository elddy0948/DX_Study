#pragma once

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <vector>
#include <array>

#include "BaseApp.h"
#include "UploadBuffer.h"
#include "MeshGeometry.h"

static const DirectX::XMFLOAT4X4 Identity4x4 =
{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f,
};

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 WorldViewProj = Identity4x4;
};

class DrawApp : public BaseApp
{
public:
	DrawApp(HINSTANCE hInstance);
	~DrawApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	void BuildConstantBufferDescriptorHeap();
	void BuildConstantBuffer();
	void BuildRootSignature();

	void BuildShaders();
	void SetInputLayout();

	void BuildBox();
	void BuildPSO();

	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

private:
	const int NumElements = 1;
	
	float m_theta = 1.5f * DirectX::XM_PI;
	float m_phi = DirectX::XM_PIDIV4;
	float m_radius = 5.0f;

	std::unique_ptr<UploadBuffer<ObjectConstants>> m_objectConstantBuffer = nullptr;
	std::unique_ptr<MeshGeometry> m_boxGeometry = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadConstantBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_constantBufferHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
	
	Microsoft::WRL::ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> m_psByteCode = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso = nullptr;

	DirectX::XMFLOAT4X4 m_world = Identity4x4;
	DirectX::XMFLOAT4X4 m_view = Identity4x4;
	DirectX::XMFLOAT4X4 m_proj = Identity4x4;
};