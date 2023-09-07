#pragma once

#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <Windows.h>
#include <unordered_map>

#include "SubmeshGeometry.h"

using namespace Microsoft::WRL;

struct MeshGeometry
{
	std::string Name;

	ComPtr<ID3DBlob> vPosBufferCPU = nullptr;
	ComPtr<ID3D12Resource> vPosBufferGPU = nullptr;
	ComPtr<ID3D12Resource> vPosBufferUploader = nullptr;

	ComPtr<ID3DBlob> vColorBufferCPU = nullptr;
	ComPtr<ID3D12Resource> vColorBufferGPU = nullptr;
	ComPtr<ID3D12Resource> vColorBufferUploader = nullptr;

	ComPtr<ID3DBlob> indexBufferCPU = nullptr;	
	ComPtr<ID3D12Resource> indexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> indexBufferUploader = nullptr;

	UINT vPosByteStride = 0;
	UINT vPosBufferByteSize = 0;
	UINT vColorByteStride = 0;
	UINT vColorBufferByteSize = 0;

	DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
	UINT indexBufferByteSize = 0;

	std::unordered_map<std::string, SubmeshGeometry> drawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexPositionBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = vPosBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = vPosByteStride;
		vbv.SizeInBytes = vPosBufferByteSize;
		return vbv;
	}

	D3D12_VERTEX_BUFFER_VIEW VertexColorBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = vColorBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = vColorByteStride;
		vbv.SizeInBytes = vColorBufferByteSize;
		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = indexFormat;
		ibv.SizeInBytes = indexBufferByteSize;
		return ibv;
	}

	void DisposeUploaders()
	{
		vertexBufferUploader = nullptr;
		indexBufferUploader = nullptr;
		vPosBufferUploader = nullptr;
		vColorBufferUploader = nullptr;
	}
};