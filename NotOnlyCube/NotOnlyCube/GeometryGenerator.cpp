#include "GeometryGenerator.h"
#include <DirectXMath.h>
#include <algorithm>
#include <corecrt_math.h>

GeometryGenerator::MeshData
GeometryGenerator::CreateCylinder(
		float bottomRadius,
		float topRadius,
		float height,
		uint32 sliceCount,
		uint32 stackCount)
{
	MeshData meshData;

	float stackHeight = height / stackCount;
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	uint32 ringCount = stackCount + 1;

	for (uint32 i = 0; i < ringCount; ++i)
	{
		float y = -0.5f * height + i * stackHeight;
		float r = bottomRadius + i * radiusStep;

		float dTheta = 2.0f * DirectX::XM_PI / sliceCount;

		for (uint32 j = 0; j <= sliceCount; ++j)
		{
			Vertex vertex;
			float c = cosf(j * dTheta);
			float s = sinf(j * dTheta);
			
			vertex.Position = DirectX::XMFLOAT3(r * c, y, r * s);
			vertex.TexC.x = (float)j / sliceCount;
			vertex.TexC.y = 1.0f - (float)i / stackCount;

			vertex.TangentU = DirectX::XMFLOAT3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			DirectX::XMFLOAT3 bittangent(dr * c, -height, dr * s);
			DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&vertex.TangentU);
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&bittangent);
			DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(T, B));
      XMStoreFloat3(&vertex.Normal, N);
      meshData.Vertices.push_back(vertex);
		}
	}
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 i = 0; i < stackCount; ++i)
	{
		for (uint32 j = 0; j < sliceCount; ++j)
		{
			meshData.Indices32.push_back(i * ringVertexCount + j);
			meshData.Indices32.push_back((i + 1) * ringVertexCount + j);
			meshData.Indices32.push_back((i + 1) * ringVertexCount + j + 1);

			meshData.Indices32.push_back(i * ringVertexCount + j);
			meshData.Indices32.push_back((i + 1) * ringVertexCount + j + 1);
			meshData.Indices32.push_back(i * ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	
	return meshData;
}

void GeometryGenerator::BuildCylinderTopCap(
	float bottomRadius,
	float topRadius,
	float height,
	uint32 sliceCount,
	uint32 stackCount,
	GeometryGenerator::MeshData& meshData
)
{
  uint32 baseIndex = (uint32)meshData.Vertices.size();
  float y = 0.5f * height;
  float dTheta = 2.0f * DirectX::XM_PI / sliceCount;

  for (uint32 i = 0; i <= sliceCount; ++i)
  {
    float x = topRadius * cosf(i * dTheta);
    float z = topRadius * sinf(i * dTheta);

    float u = x / height + 0.5f;
    float v = z / height + 0.5f;

    meshData.Vertices.push_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
  }

  meshData.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

  uint32 centerIndex = (uint32)meshData.Vertices.size() - 1;

  for (uint32 i = 0; i < sliceCount; ++i)
  {
    meshData.Indices32.push_back(centerIndex);
    meshData.Indices32.push_back(baseIndex + i + 1);
    meshData.Indices32.push_back(baseIndex + i);
  }
}

void GeometryGenerator::BuildCylinderBottomCap(
	float bottomRadius,
	float topRadius,
	float height,
	uint32 sliceCount,
	uint32 stackCount,
	GeometryGenerator::MeshData& meshData
)
{
  uint32 baseIndex = (uint32)meshData.Vertices.size();
  float y = -0.5f * height;
  float dTheta = 2.0f * DirectX::XM_PI / sliceCount;

  for (uint32 i = 0; i <= sliceCount; ++i)
  {
    float x = bottomRadius * cosf(i * dTheta);
    float z = bottomRadius * sinf(i * dTheta);

    float u = x / height + 0.5f;
    float v = z / height + 0.5f;

    meshData.Vertices.push_back(Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
  }
  meshData.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));
  uint32 centerIndex = (uint32)meshData.Vertices.size() - 1;
  for (uint32 i = 0; i < sliceCount; ++i)
  {
    meshData.Indices32.push_back(centerIndex);
    meshData.Indices32.push_back(baseIndex + i);
    meshData.Indices32.push_back(baseIndex + i + 1);
  }
}

GeometryGenerator::MeshData GeometryGenerator::CreateSphere(
  float radius,
  uint32 sliceCount,
  uint32 stackCount
)
{
  MeshData meshData;

  Vertex topVertex(0.0f, radius, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
  Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

  meshData.Vertices.push_back(topVertex);

  float phiStep = DirectX::XM_PI / stackCount;
  float thetaStep = 2.0f * DirectX::XM_PI / sliceCount;

  for (uint32 i = 1; i <= stackCount - 1; ++i)
  {
    float phi = i * phiStep;

    for (uint32 j = 0; j <= sliceCount; ++j)
    {
      float theta = j * thetaStep;
      Vertex v;

      v.Position.x = radius * sinf(phi) * cosf(theta);
      v.Position.y = radius * cosf(phi);
      v.Position.z = radius * sinf(phi) * sinf(theta);

      v.TangentU.x = -radius * sinf(phi) * sinf(theta);
      v.TangentU.y = 0.0f;
      v.TangentU.z = radius * sinf(phi) * cosf(theta);
      
      DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&v.TangentU);
      DirectX::XMStoreFloat3(&v.TangentU, DirectX::XMVector3Normalize(T));
      
      DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&v.Position);
      DirectX::XMStoreFloat3(&v.Normal, DirectX::XMVector3Normalize(P));

      v.TexC.x = theta / DirectX::XM_2PI;
      v.TexC.y = phi / DirectX::XM_PI;

      meshData.Vertices.push_back(v);
    }
  }

  meshData.Vertices.push_back(bottomVertex);

  for (uint32 i = 1; i <= sliceCount; ++i)
  {
    meshData.Indices32.push_back(0);
    meshData.Indices32.push_back(i + 1);
    meshData.Indices32.push_back(i);
  }

  uint32 baseIndex = 1;
  uint32 ringVertexCount = sliceCount + 1;

  for (uint32 i = 0; i < stackCount - 2; ++i)
  {
    for (uint32 j = 0; j < sliceCount; ++j)
    {
      meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j);
      meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
      meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);

      meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j);
      meshData.Indices32.push_back(baseIndex + i * ringVertexCount + j + 1);
      meshData.Indices32.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
    }
  }
  uint32 southPoleIndex = (uint32)meshData.Vertices.size() - 1;

  baseIndex = southPoleIndex - ringVertexCount;

  for (uint32 i = 0; i < sliceCount; ++i)
  {
    meshData.Indices32.push_back(southPoleIndex);
    meshData.Indices32.push_back(baseIndex + i);
    meshData.Indices32.push_back(baseIndex + i + 1);
  }

  return meshData;
}

GeometryGenerator::MeshData GeometryGenerator::CreateGeoSphere(
  float radius,
  uint32 numSubdivisioins
)
{
  MeshData meshData;

  numSubdivisioins = std::min<uint32>(numSubdivisioins, 6u);

  const float X = 0.525731f;
  const float Z = 0.850651f;

  DirectX::XMFLOAT3 pos[12] = 
  {
    DirectX::XMFLOAT3(-X, 0.0f, Z), DirectX::XMFLOAT3(X, 0.0f, Z),
    DirectX::XMFLOAT3(-X, 0.0f, -Z), DirectX::XMFLOAT3(X, 0.0f, -Z),
    DirectX::XMFLOAT3(0.0f, Z, X), DirectX::XMFLOAT3(0.0f, Z, -X),
    DirectX::XMFLOAT3(0.0f, -Z, X), DirectX::XMFLOAT3(0.0f, -Z, -X),
    DirectX::XMFLOAT3(Z, X, 0.0f), DirectX::XMFLOAT3(-Z, X, 0.0f),
    DirectX::XMFLOAT3(Z, -X, 0.0f), DirectX::XMFLOAT3(-Z, -X, 0.0f)
  };

  uint32 k[60] =
  {
      1, 4, 0,  4, 9, 0,  4, 5, 9,  8, 5, 4,  1, 8, 4,
      1, 10, 8,  10, 3, 8,  8, 3, 5,  3, 2, 5,  3, 7, 2,
      3, 10, 7,  10, 6, 7,  6, 11, 7,  6, 0, 11,  6, 1, 0,
      10, 1, 6,  11, 0, 9,  2, 11, 9,  5, 2, 9,  11, 2, 7
  };

  meshData.Vertices.resize(12);
  meshData.Indices32.assign(&k[0], &k[60]);

  for (uint32 i = 0; i < 12; ++i)
    meshData.Vertices[i].Position = pos[i];

  for (uint32 i = 0; i < numSubdivisioins; ++i)
    Subdivide(meshData);
}

GeometryGenerator::Vertex GeometryGenerator::MidPoint(const Vertex& v0, const Vertex& v1)
{
    using namespace DirectX;

    XMVECTOR p0 = XMLoadFloat3(&v0.Position);
    XMVECTOR p1 = XMLoadFloat3(&v1.Position);

    XMVECTOR n0 = XMLoadFloat3(&v0.Normal);
    XMVECTOR n1 = XMLoadFloat3(&v1.Normal);

    XMVECTOR t0 = XMLoadFloat3(&v0.TangentU);
    XMVECTOR t1 = XMLoadFloat3(&v1.TangentU);
}