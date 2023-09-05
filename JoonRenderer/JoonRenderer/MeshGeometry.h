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

	ComPtr<ID3DBlob> vertexBufferCPU = nullptr;
	ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;

	ComPtr<ID3DBlob> indexBufferCPU = nullptr;	
	ComPtr<ID3D12Resource> indexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> indexBufferUploader = nullptr;

	UINT vertexByteStride = 0;
	UINT vertexBufferByteSize = 0;

	DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
	UINT indexBufferByteSize = 0;

	std::unordered_map<std::string, SubmeshGeometry> drawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = vertexByteStride;
		vbv.SizeInBytes = vertexBufferByteSize;
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
	}
};