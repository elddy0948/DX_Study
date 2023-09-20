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

	// NumFrameResource global ������ �޾ƿ���
	int NumFramesDirty = NumFrameResource;

	// Shader�� ����� Constant buffer �����͵�
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f }; // 0�� �������� �л��� ���� ���Ͼ.
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;
	DirectX::XMFLOAT4X4 MaterialTransform = JRMath::Identity4x4();
};