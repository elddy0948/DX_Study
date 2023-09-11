#pragma once

#include "UploadBuffer.h"

struct PassConstants
{

};

struct ObjectConstants
{

};

struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;

	std::unique_ptr<UploadBuffer<PassConstants>> passConstantBuffer = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> objectConstantBuffer = nullptr;

	UINT64 fence = 0;
}; 