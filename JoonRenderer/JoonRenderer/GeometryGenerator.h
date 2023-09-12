#pragma once


// Introduction to 3d game programming with directx12 Âü°í
#include <vector>
#include <algorithm>

#include "Helper.h"

class GeometryGenerator
{
public:
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;

	struct Vertex
	{
		Vertex() {}
		Vertex(
			const DirectX::XMFLOAT3& p,
			const DirectX::XMFLOAT3& n,
			const DirectX::XMFLOAT3& t,
			const DirectX::XMFLOAT2& uv
		) : position(p), normal(n), tangentU(t), texC(uv) {}

		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v
		) : position(px, py, pz), normal(nx, ny, nz), tangentU(tx, ty, tz), texC(u, v) {}


		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangentU;
		DirectX::XMFLOAT2 texC;
	};

	struct MeshData
	{
		std::vector<Vertex> vertices;
		std::vector<uint32> indices32;

		std::vector<uint16>& GetIndices16()
		{
			if (m_indices16.empty())
			{
				m_indices16.resize(indices32.size());
				for (size_t i = 0; i < indices32.size(); ++i)
				{
					m_indices16[i] = static_cast<uint16>(indices32[i]);
				}
			}

			return m_indices16;
		}

	private:
		std::vector<uint16> m_indices16;
	};

	// ...

protected:
	MeshData CreateBox(float width, float height, float depth, uint32 numSubdivisions);
	MeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);
	MeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);
	MeshData CreateGeoSphere(float radius, uint32 numSubdivisions);

private:
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, MeshData& meshData);
	void Subdivide(MeshData& meshData);
	Vertex MidPoint(const Vertex& v0, const Vertex& v1);
};