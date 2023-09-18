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

	// �����Ӻ��� command allocator�� �ξ�� GPU�� ����� ��� ó���ϱ� ���� command allocator�� �缳�� �ϴ� ���� �߻����� �ʴ´�.
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;

	// constant buffer ���� �̸� ó���ϰ� �ִ� GPU�� �� ó���ϱ� ���� �缳���ϸ� �ȵȴ�. �׷��Ƿ� �����Ӻ��� constant buffer�� �д�.
	std::unique_ptr<UploadBuffer<PassConstants>> passConstantBuffer = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> objectConstantBuffer = nullptr;

	std::unique_ptr<UploadBuffer<VertexPositionData>> vertexPositionBuffer = nullptr;
	std::unique_ptr<UploadBuffer<VertexColorData>> vertexColorBuffer = nullptr;

	UINT64 fence = 0;
};