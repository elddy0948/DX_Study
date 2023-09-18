#pragma once

#include "BaseApp.h"
#include "Helper.h"
#include "GeometryGenerator.h"
#include "MeshGeometry.h"

using namespace DirectX;

struct VertexPositionData
{
	XMFLOAT3 position;
};

struct VertexColorData
{
	XMFLOAT4 color;
};

class LandAndWavesApp : public BaseApp
{
public:
	LandAndWavesApp(HINSTANCE hInstance);
	LandAndWavesApp(const LandAndWavesApp& rhs) = delete;
	LandAndWavesApp& operator=(const LandAndWavesApp& rhs) = delete;
	~LandAndWavesApp();

private:
	void BuildLandGeometry();
	float GetHeight(float x, float z) const;


private:
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
}; 