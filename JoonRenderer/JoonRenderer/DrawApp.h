#pragma once

#include "BaseApp.h"

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

	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* commandList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;
};