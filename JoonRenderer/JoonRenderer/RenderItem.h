#pragma once

#include "Helper.h"
#include "MeshGeometry.h"
#include "FrameResource.h"

using namespace DirectX;

struct RenderItem
{
	RenderItem() = default;

	XMFLOAT4X4 inWorldSpace = Identity4x4;

	int constantBufferChangeFlag = NumFrameResources;

	UINT objectConstantBufferIndex = -1;

	MeshGeometry* geo = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// for DrawIndexedInstanced parameters
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	int baseVertexLocation = 0;
};