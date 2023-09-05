#pragma once

#include <DirectXMath.h>
#include <DirectXColors.h>

#include "BaseApp.h"

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
	void SetInputLayout();
	void ConfigureVertexBuffer();
	void ConfigureIndexBuffer();
	void ConfigureConstantBuffer();

	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

private:
	const int NumElements = 1;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadConstantBuffer = nullptr;
};