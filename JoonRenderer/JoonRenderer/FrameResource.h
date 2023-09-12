#pragma once

#include "UploadBuffer.h"

static const int NumFrameResources = 3;

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

struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();
	
	// �����Ӻ��� command allocator�� �ξ�� GPU�� ����� ��� ó���ϱ� ���� command allocator�� �缳�� �ϴ� ���� �߻����� �ʴ´�.
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;

	// constant buffer ���� �̸� ó���ϰ� �ִ� GPU�� �� ó���ϱ� ���� �缳���ϸ� �ȵȴ�. �׷��Ƿ� �����Ӻ��� constant buffer�� �д�.
	std::unique_ptr<UploadBuffer<PassConstants>> passConstantBuffer = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> objectConstantBuffer = nullptr;

	UINT64 fence = 0;
}; 