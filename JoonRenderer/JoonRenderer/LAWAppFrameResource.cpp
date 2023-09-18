#include "LAWAppFrameResource.h"

LAWAppFrameResource::LAWAppFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT waveVertCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())));
	passConstantBuffer = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	objectConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
	vertexPositionBuffer = std::make_unique<UploadBuffer<VertexPositionData>>(device, waveVertCount, false);
	vertexColorBuffer = std::make_unique<UploadBuffer<VertexColorData>>(device, waveVertCount, false);
}

LAWAppFrameResource::~LAWAppFrameResource() {}