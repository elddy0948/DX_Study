#pragma once

#include "UploadBuffer.h"

const int NumFrameResources = 3;

struct PassConstants
{
	XMFLOAT4X4 view;
	XMFLOAT4X4 invView;
	XMFLOAT4X4 proj;
	XMFLOAT4X4 invProj;
	XMFLOAT4X4 viewProj;
	XMFLOAT4X4 invViewProj;

	XMFLOAT3 eyePos;
	XMFLOAT2 renderTargetSize;
	XMFLOAT2 invRenderTargetSize;

	float nearZ;
	float farZ;
	float totalTime;
	float deltaTime;
};

struct ObjectConstants
{
	XMFLOAT4X4 world;
};

struct VertexPositionData
{
	DirectX::XMFLOAT3 position;
};

struct VertexColorData
{
	DirectX::XMFLOAT4 color;
};

struct LAWAppFrameResource
{
public:
	LAWAppFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount);
	LAWAppFrameResource(const LAWAppFrameResource& rhs) = delete;
	LAWAppFrameResource& operator=(const LAWAppFrameResource& rhs) = delete;
	~LAWAppFrameResource();

	// 프레임별로 command allocator를 두어야 GPU가 명령을 모두 처리하기 전에 command allocator를 재설정 하는 일이 발생하지 않는다.
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;

	// constant buffer 또한 이를 처리하고 있는 GPU가 다 처리하기 전에 재설정하면 안된다. 그러므로 프레임별로 constant buffer를 둔다.
	std::unique_ptr<UploadBuffer<PassConstants>> passConstantBuffer = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> objectConstantBuffer = nullptr;

	std::unique_ptr<UploadBuffer<VertexPositionData>> vertexPositionBuffer = nullptr;
	std::unique_ptr<UploadBuffer<VertexColorData>> vertexColorBuffer = nullptr;

	UINT64 fence = 0;
};