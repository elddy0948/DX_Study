#include "LandAndWavesApp.h"

LandAndWavesApp::LandAndWavesApp(HINSTANCE hInstance) : BaseApp(hInstance) {}

LandAndWavesApp::~LandAndWavesApp() { BaseApp::~BaseApp(); }

void LandAndWavesApp::BuildLandGeometry()
{
	GeometryGenerator geoGen;

	GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);

	std::vector<VertexPositionData> vertexPositions(grid.vertices.size());
	std::vector<VertexColorData> vertexColors(grid.vertices.size());

	for (size_t i = 0; i < grid.vertices.size(); ++i)
	{
		auto& p = grid.vertices[i].position;

		vertexPositions[i].position = p;
		vertexPositions[i].position.y = GetHeight(p.x, p.y);

		if (vertexPositions[i].position.y < -10.0f)
		{
			vertexColors[i].color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if (vertexPositions[i].position.y < 5.0f)
		{
			vertexColors[i].color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if (vertexPositions[i].position.y < 12.0f)
		{
			vertexColors[i].color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}
		else
		{
			vertexColors[i].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	const UINT vpbByteSize = (UINT)vertexPositions.size() * sizeof(VertexPositionData);
	const UINT vcbByteSize = (UINT)vertexColors.size() * sizeof(VertexColorData);

	std::vector<std::uint16_t> indices = grid.GetIndices16();

	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "LandGeo";

	ThrowIfFailed(D3DCreateBlob(vpbByteSize, &geo->vPosBufferCPU));
	CopyMemory(geo->vPosBufferCPU->GetBufferPointer(), vertexPositions.data(), vpbByteSize);
	
	ThrowIfFailed(D3DCreateBlob(vcbByteSize, &geo->vColorBufferCPU));
	CopyMemory(geo->vColorBufferCPU->GetBufferPointer(), vertexColors.data(), vcbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->indexBufferCPU));
	CopyMemory(geo->indexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->vPosBufferGPU = Helper::CreateDefaultBuffer(m_device.Get(), m_commandList.Get(), vertexPositions.data(), vpbByteSize, geo->vPosBufferUploader);
	geo->vColorBufferGPU = Helper::CreateDefaultBuffer(m_device.Get(), m_commandList.Get(), vertexColors.data(), vcbByteSize, geo->vColorBufferUploader);
	geo->indexBufferGPU = Helper::CreateDefaultBuffer(m_device.Get(), m_commandList.Get(), indices.data(), ibByteSize, geo->indexBufferUploader);

	geo->vPosByteStride = sizeof(VertexPositionData);
	geo->vColorByteStride = sizeof(VertexColorData);
	geo->vPosBufferByteSize = vpbByteSize;
	geo->vColorBufferByteSize = vcbByteSize;

	geo->indexFormat = DXGI_FORMAT_R16_UINT;
	geo->indexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->drawArgs["grid"] = submesh;

	m_geometries["LandGeo"] = std::move(geo);
}

float LandAndWavesApp::GetHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}
