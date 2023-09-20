#pragma once

#include <DirectXMath.h>
#include <string>

#include "JRMath.h"

const int NumFrameResource = 3;

struct Material
{
	std::string Name;

	int ConstantBufferIndex = -1;
	int DiffuseSRVHeapIndex = -1;

	// NumFrameResource global 변수로 받아오기
	int NumFramesDirty = NumFrameResource;

	// Shader가 사용할 Constant buffer 데이터들
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f }; // 0에 가까울수록 분산이 많이 안일어남.
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;
	DirectX::XMFLOAT4X4 MaterialTransform = JRMath::Identity4x4();
};