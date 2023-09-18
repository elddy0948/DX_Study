#include "LandAndWavesApp.h"

LandAndWavesApp::LandAndWavesApp(HINSTANCE hInstance) : BaseApp(hInstance) {}

LandAndWavesApp::~LandAndWavesApp() { BaseApp::~BaseApp(); }

void LandAndWavesApp::Draw()
{
	// ...

	auto passCB = m_currentFrameResource->passConstantBuffer->Resource();
	m_commandList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());
	DrawRenderItems(m_commandList.Get(), m_opaqueRenderItems);
	
	// ...
}

void LandAndWavesApp::ConfigureRootSignature()
{
	CD3DX12_ROOT_PARAMETER rootParameterSlots[2] = {};
	rootParameterSlots[0].InitAsConstantBufferView(0); // object cb
	rootParameterSlots[1].InitAsConstantBufferView(1); // pass cb

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(2, rootParameterSlots, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
}


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

void LandAndWavesApp::DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<LAWAppRenderItem*>& renderItems)
{
	UINT objectCBByteSize = Helper::CalculateConstantBufferByteSize(sizeof(ObjectConstants));
	auto objectCB = m_currentFrameResource->objectConstantBuffer->Resource();

	for (size_t i = 0; i < renderItems.size(); ++i)
	{
		auto renderItem = renderItems[i];

		D3D12_VERTEX_BUFFER_VIEW vbv[] =
		{
			renderItem->geo->VertexPositionBufferView(),
			renderItem->geo->VertexColorBufferView()
		};
		commandList->IASetVertexBuffers(0, 2, vbv);
		commandList->IASetIndexBuffer(&renderItem->geo->IndexBufferView());
		commandList->IASetPrimitiveTopology(renderItem->primitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objectCBAddress = objectCB->GetGPUVirtualAddress();
		objectCBAddress += renderItem->objectConstantBufferIndex * objectCBByteSize;
		commandList->SetGraphicsRootConstantBufferView(0, objectCBAddress);
		commandList->DrawIndexedInstanced(renderItem->indexCount, 1, renderItem->startIndexLocation, renderItem->baseVertexLocation, 0);
	}
}

void LandAndWavesApp::UpdateWaves()
{
	static float t_base = 0.0f;

	if ((Win32Application::GetTimer().GameTime() - t_base) >= 0.25f)
	{
		t_base += 0.25f;

		int i = Rand(4, m_waves->RowCount() - 5);
		int j = Rand(4, m_waves->ColumnCount() - 5);

		float r = RandF(0.2f, 0.5f);

		m_waves->Disturb(i, j, r);
	}

	m_waves->Update(Win32Application::GetTimer().DeltaTime());

	auto currentWavesVPB = m_currentFrameResource->vertexPositionBuffer.get();
	auto currentWavesVCB = m_currentFrameResource->vertexColorBuffer.get();

	for (int i = 0; i < m_waves->VertexCount(); ++i)
	{
		VertexPositionData vp;
		VertexColorData vc;

		vp.position = m_waves->Position(i);
		vc.color = XMFLOAT4(DirectX::Colors::Blue);

		currentWavesVPB->CopyData(i, vp);
		currentWavesVCB->CopyData(i, vc);
	}

	m_wavesRenderItem->geo->vPosBufferGPU = currentWavesVPB->Resource();
	m_wavesRenderItem->geo->vColorBufferGPU = currentWavesVCB->Resource();
}

float LandAndWavesApp::GetHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

int LandAndWavesApp::Rand(int a, int b)
{
	return a + rand() % ((b - a) + 1);
}

float LandAndWavesApp::RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

float LandAndWavesApp::RandF(float a, float b)
{
	return a + RandF() * (b - a);
}