#pragma once

#include "D3DHelper.h"
#include "D3DApp.h"
#include "UploadBuffer.h"

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 WorldViewProj =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
};

class DrawApp : public D3DApp
{
public:
	DrawApp(HINSTANCE hInstance);
	DrawApp(const DrawApp& rhs) = delete;
	DrawApp& operator=(const DrawApp& rhs) = delete;
	~DrawApp();

	virtual bool Initialize() override;
private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

	void SetDescriptorHeaps();
	void SetConstantBuffer();
	void SetRootSignature();
	void SetShaderAndInputLayout();
	void CreateBoxGeometry();
	void PSO();

private:	
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCBVHeap = nullptr;

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectConstantBuffer;

	std::unique_ptr<MeshGeometry> mBoxGeometry = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> mVSByteCode = nullptr; // Vertex shader byte code
	Microsoft::WRL::ComPtr<ID3DBlob> mPSByteCode = nullptr;	// Pixel shader byte code

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO = nullptr;

	DirectX::XMFLOAT4X4 mWorld =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	DirectX::XMFLOAT4X4 mView =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	DirectX::XMFLOAT4X4 mProj =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	float mTheta = 1.5f * DirectX::XM_PI;
	float mPi = DirectX::XM_PIDIV4;
	float mRadius = 5.0f;
};