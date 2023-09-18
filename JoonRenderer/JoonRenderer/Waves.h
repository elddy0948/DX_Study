#pragma once

#include <vector>
#include <DirectXMath.h>

// In 'Introducing Game Programming with DirectX12'
class Waves
{
public:
	Waves(int m, int n, float dx, float dt, float speed, float damping);
	Waves(const Waves& rhs) = delete;
	Waves& operator=(const Waves& rhs) = delete;
	~Waves();

	int RowCount() const;
	int ColumnCount() const;

	int VertexCount() const;
	int TriangleCount() const;

	float Width() const;
	float Depth() const;

	const DirectX::XMFLOAT3& Position(int i) const { return m_currentSolution[i]; }
	const DirectX::XMFLOAT3& Normal(int i) const { return m_normals[i]; }
	const DirectX::XMFLOAT3& TangentX(int i) const { return m_tangentX[i]; }

	void Update(float dt);
	void Disturb(int i, int j, float magnitude);

private:
	int m_numRows = 0;
	int m_numCols = 0;

	int m_vertexCount = 0;
	int m_triangleCount = 0;

	float m_k1 = 0.0f;
	float m_k2 = 0.0f;
	float m_k3 = 0.0f;

	float m_timeStep = 0.0f;
	float m_spatialStep = 0.0f;

	std::vector<DirectX::XMFLOAT3> m_prevSolution;
	std::vector<DirectX::XMFLOAT3> m_currentSolution;
	std::vector<DirectX::XMFLOAT3> m_normals;
	std::vector<DirectX::XMFLOAT3> m_tangentX;
};