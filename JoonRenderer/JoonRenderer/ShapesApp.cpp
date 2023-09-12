#include "ShapesApp.h"

#include <d3dcommon.h>
#include <DirectXColors.h>

ShapesApp::~ShapesApp()
{
	BaseApp::~BaseApp();
}

void ShapesApp::BuildFrameResources()
{
	for (int i = 0; i < NumFrameResources; ++i)
	{
		m_frameResources.push_back(std::make_unique<FrameResource>(m_device.Get(), 1, (UINT)m_allRenderItems.size()));
	}
}

void ShapesApp::UpdateObjectConstantBuffers()
{
	// ���� ��ü�� Constant buffer
	auto currentObjectConstantBuffer = m_currentFrameResource->objectConstantBuffer.get();

	// ���� Frame�� ��� Render item�鿡 ���ؼ�, Constant Buffer�� ������ �ִٸ� �������ش�.
	for (auto& e : m_allRenderItems)
	{
		if (e->constantBufferChangeFlag > 0)
		{
			// ���� ObjectConstants���� ����� ���� world ���̹Ƿ�, world ��ǥ���� ���� ������Ʈ ���ش�.
			XMMATRIX world = XMLoadFloat4x4(&e->inWorldSpace);

			ObjectConstants objectConstants;
			XMStoreFloat4x4(&objectConstants.world, XMMatrixTranspose(world));

			currentObjectConstantBuffer->CopyData(e->objectConstantBufferIndex, objectConstants);

			e->constantBufferChangeFlag--;
		}
	}
}

// ������ �н��� �ѹ��� ���ŵ� pass constant buffer�� ���� �Լ�
void ShapesApp::UpdateMainPassConstantBuffer()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	XMMATRIX proj = XMLoadFloat4x4(&m_proj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&m_mainPassConstantBuffer.view, XMMatrixTranspose(view));
	XMStoreFloat4x4(&m_mainPassConstantBuffer.invView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&m_mainPassConstantBuffer.proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&m_mainPassConstantBuffer.invProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&m_mainPassConstantBuffer.viewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&m_mainPassConstantBuffer.invViewProj, XMMatrixTranspose(invViewProj));

	m_mainPassConstantBuffer.eyePos = m_eyePos;
	m_mainPassConstantBuffer.renderTargetSize = XMFLOAT2((float)m_clientWidth, (float)m_clientHeight);
	m_mainPassConstantBuffer.invRenderTargetSize = XMFLOAT2(1.0f / m_clientWidth, 1.0f / m_clientHeight);
	m_mainPassConstantBuffer.nearZ = 1.0f;
	m_mainPassConstantBuffer.farZ = 1000.0f;
	m_mainPassConstantBuffer.totalTime = Win32Application::GetTimer().GameTime();
	m_mainPassConstantBuffer.deltaTime = Win32Application::GetTimer().DeltaTime();

	auto currentPassConstantBuffer = m_currentFrameResource->passConstantBuffer.get();
	currentPassConstantBuffer->CopyData(0, m_mainPassConstantBuffer);
}

void ShapesApp::ConfigureRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE cbvTable0;
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE cbvTable1;
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	CD3DX12_ROOT_PARAMETER slotRootParameters[2];

	slotRootParameters[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameters[1].InitAsDescriptorTable(1, &cbvTable1);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(2, slotRootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
}

void ShapesApp::BuildShapeGeometry()
{
	GeometryGenerator geoGenerator;

	GeometryGenerator::MeshData box = geoGenerator.CreateBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData grid = geoGenerator.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGenerator.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGenerator.CreateCylinder(0.5f, 0.3f, 0.3f, 20, 20);

	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.vertices.size();

	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.indices32.size();

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	auto totalVertexCount = box.vertices.size() + grid.vertices.size() + sphere.vertices.size() + cylinder.vertices.size();

	std::vector<VertexPositionData> verticesPosition(totalVertexCount);
	std::vector<VertexColorData> verticesColor(totalVertexCount);

	UINT k = 0;

	for (size_t i = 0; i < box.vertices.size(); ++i)
	{
		verticesPosition[k].position = box.vertices[i].position;
		verticesColor[k].color = XMFLOAT4(DirectX::Colors::DarkGreen);
		k++;
	}

	for (size_t i = 0; i < grid.vertices.size(); ++i)
	{
		verticesPosition[k].position = grid.vertices[i].position;
		verticesColor[k].color = XMFLOAT4(DirectX::Colors::ForestGreen);
		k++;
	}

	for (size_t i = 0; i < sphere.vertices.size(); ++i)
	{
		verticesPosition[k].position = sphere.vertices[i].position;
		verticesColor[k].color = XMFLOAT4(DirectX::Colors::Crimson);
		k++;
	}

	for (size_t i = 0; i < cylinder.vertices.size(); ++i)
	{
		verticesPosition[k].position = cylinder.vertices[i].position;
		verticesColor[k].color = XMFLOAT4(DirectX::Colors::SteelBlue);
		k++;
	}

	std::vector<std::uint16_t> indices;

	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vPosBufferByteSize = (UINT)verticesPosition.size() * sizeof(VertexPositionData);
	const UINT vColorBufferByteSize = (UINT)verticesColor.size() * sizeof(VertexColorData);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "ShapeGeo";

	ThrowIfFailed(D3DCreateBlob(vPosBufferByteSize, &geo->vPosBufferCPU));
	ThrowIfFailed(D3DCreateBlob(vColorBufferByteSize, &geo->vColorBufferCPU));
	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->indexBufferCPU));

	CopyMemory(geo->vPosBufferCPU->GetBufferPointer(), verticesPosition.data(), vPosBufferByteSize);
	CopyMemory(geo->vColorBufferCPU->GetBufferPointer(), verticesColor.data(), vColorBufferByteSize);
	CopyMemory(geo->indexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->vPosBufferGPU = Helper::CreateDefaultBuffer(m_device.Get(), m_commandList.Get(), verticesPosition.data(), vPosBufferByteSize, geo->vPosBufferUploader);
	geo->vColorBufferGPU = Helper::CreateDefaultBuffer(m_device.Get(), m_commandList.Get(), verticesColor.data(), vColorBufferByteSize, geo->vColorBufferUploader);
	geo->indexBufferGPU = Helper::CreateDefaultBuffer(m_device.Get(), m_commandList.Get(), indices.data(), ibByteSize, geo->indexBufferUploader);

	geo->vPosByteStride = sizeof(VertexPositionData);
	geo->vPosBufferByteSize = vPosBufferByteSize;
	geo->vColorByteStride = sizeof(VertexColorData);
	geo->vColorBufferByteSize = vColorBufferByteSize;
	geo->indexFormat = DXGI_FORMAT_R16_UINT;
	geo->indexBufferByteSize = ibByteSize;

	geo->drawArgs["box"] = boxSubmesh;
	geo->drawArgs["grid"] = gridSubmesh;
	geo->drawArgs["sphere"] = sphereSubmesh;
	geo->drawArgs["cylinder"] = cylinderSubmesh;

	m_geometries[geo->Name] = std::move(geo);
}

void ShapesApp::Update()
{
	// ���� FrameResource�� ����
	m_currentFrameResourceIndex = (m_currentFrameResourceIndex + 1) % NumFrameResources;
	m_currentFrameResource = m_frameResources[m_currentFrameResourceIndex].get();

	// Fence�� ���� ���� ���� GPU�� �ش� resource�� ������ �۾� ������ Ȯ���ϰ�, ������̶�� ��ٸ���.
	if (m_currentFrameResource->fence != 0 && m_fence->GetCompletedValue() < m_currentFrameResource->fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		ThrowIfFailed(m_fence->SetEventOnCompletion(m_currentFrameResource->fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	// m_currentFrameResource�� �ڿ����� �����Ѵ�.
}

void ShapesApp::Draw()
{
	m_currentFrameResource->fence = ++m_currentFence;
	m_commandQueue->Signal(m_fence.Get(), m_currentFence);
}