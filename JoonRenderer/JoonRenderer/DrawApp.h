#pragma once

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <vector>
#include <array>

#include "BaseApp.h"
#include "UploadBuffer.h"
#include "MeshGeometry.h"

using namespace DirectX;

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = Identity4x4;
	float gTime = 0.0f;
};

struct VPosData
{
	XMFLOAT3 Pos;
};

struct VColorData
{
	XMFLOAT4 Color;
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

	virtual void OnLeftKeyDown() override;
	virtual void OnRightKeyDown() override;
	virtual void OnUpKeyDown() override;
	virtual void OnDownKeyDown() override;

private:
	void BuildConstantBufferDescriptorHeap();
	void BuildConstantBuffer();
	void BuildRootSignature();

	void BuildShaders();
	void SetInputLayout();

	void BuildGeo();
	void BuildPSO();

private:
	float m_theta = 1.5f * XM_PI;
	float m_phi = XM_PIDIV4;
	float m_radius = 5.0f;

	std::unique_ptr<UploadBuffer<ObjectConstants>> m_objectConstantBuffer = nullptr;
	std::unique_ptr<MeshGeometry> m_geo = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_constantBufferHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;
	
	Microsoft::WRL::ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> m_psByteCode = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pso = nullptr;

	XMFLOAT4X4 m_world = Identity4x4;
	XMFLOAT4X4 m_view = Identity4x4;
	XMFLOAT4X4 m_proj = Identity4x4;
};