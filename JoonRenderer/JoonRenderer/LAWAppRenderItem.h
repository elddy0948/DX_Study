#pragma once

#include "LAWAppFrameResource.h"
#include "MeshGeometry.h"
#include "JRMath.h"

struct LAWAppRenderItem
{
	LAWAppRenderItem() = default;

	XMFLOAT4X4 inWorldSpace = JRMath::Identity4x4();

	int constantBufferChangeFlag = NumFrameResources;

	UINT objectConstantBufferIndex = -1;

	MeshGeometry* geo = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	int baseVertexLocation = 0;
};