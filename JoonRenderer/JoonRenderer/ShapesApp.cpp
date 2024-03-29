#include "ShapesApp.h"

#include <d3dcommon.h>
#include <DirectXColors.h>

ShapesApp::ShapesApp(HINSTANCE hInstance) : BaseApp(hInstance) {}

ShapesApp::~ShapesApp()
{
	if (m_device != nullptr)
		FlushCommandQueue();
}

bool ShapesApp::Initialize()
{
	if (!BaseApp::Initialize()) { return false; }

	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	ConfigureRootSignature();
	BuildShadersAndInputLayout();
	BuildShapeGeometry();
	BuildRenderItems();
	BuildFrameResources();
	BuildDescriptorHeaps();
	BuildConstantBufferViews();
	BuildPSOs();

	ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	return true;
}

void ShapesApp::OnResize()
{
	BaseApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25 * 3.141592f, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_proj, P);
}

void ShapesApp::Update()
{
	// 다음 FrameResource에 접근
	m_currentFrameResourceIndex = (m_currentFrameResourceIndex + 1) % NumFrameResources;
	m_currentFrameResource = m_frameResources[m_currentFrameResourceIndex].get();

	// Fence의 값을 보고 현재 GPU가 해당 resource를 가지고 작업 중인지 확인하고, 사용중이라면 기다린다.
	if (m_currentFrameResource->fence != 0 && m_fence->GetCompletedValue() < m_currentFrameResource->fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		ThrowIfFailed(m_fence->SetEventOnCompletion(m_currentFrameResource->fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	// m_currentFrameResource의 자원들을 갱신한다.
	UpdateObjectConstantBuffers();
	UpdateMainPassConstantBuffer();
}

void ShapesApp::Draw()
{
	auto commandAllocator = m_currentFrameResource->commandAllocator;

	ThrowIfFailed(commandAllocator->Reset());

	if (m_IsWireFrame)
	{
		ThrowIfFailed(m_commandList->Reset(commandAllocator.Get(), m_PSOs["opaque_wireframe"].Get()));
	}
	else
	{
		ThrowIfFailed(m_commandList->Reset(commandAllocator.Get(), m_PSOs["opaque"].Get()));
	}

	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	m_commandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	m_commandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_commandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	// descriptor table을 pipeline에 묶는다.
	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_cbvHeap.Get() };
	m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// Object CBV Descriptor table 묶기
	auto objectCBVHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	objectCBVHandle.Offset(0, m_cbvsrvDescriptorSize);
	m_commandList->SetGraphicsRootDescriptorTable(0, objectCBVHandle);

	// Pass CBV Descriptor table 묶기
	int passCBVIndex = m_passCBVOffset + m_currentFrameResourceIndex;
	auto passCBVHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	passCBVHandle.Offset(passCBVIndex, m_cbvsrvDescriptorSize);
	m_commandList->SetGraphicsRootDescriptorTable(1, passCBVHandle);

	DrawRenderItems(m_commandList.Get(), m_opaqueRenderItems);

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// 후면버퍼와 전면버퍼 교환
	ThrowIfFailed(m_swapChain->Present(0, 0));
	m_currentBackBuffer = (m_currentBackBuffer + 1) % SwapChainBufferCount;

	m_currentFrameResource->fence = ++m_currentFence;
	m_commandQueue->Signal(m_fence.Get(), m_currentFence);
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
	// 현재 객체의 Constant buffer
	auto currentObjectConstantBuffer = m_currentFrameResource->objectConstantBuffer.get();

	// 현재 Frame의 모든 Render item들에 대해서, Constant Buffer의 변경이 있다면 변경해준다.
	for (auto& e : m_allRenderItems)
	{
		if (e->constantBufferChangeFlag > 0)
		{
			// 현재 ObjectConstants에는 변경될 값이 world 뿐이므로, world 좌표계의 값을 업데이트 해준다.
			XMMATRIX world = XMLoadFloat4x4(&e->inWorldSpace);

			ObjectConstants objectConstants;
			XMStoreFloat4x4(&objectConstants.world, XMMatrixTranspose(world));

			currentObjectConstantBuffer->CopyData(e->objectConstantBufferIndex, objectConstants);

			e->constantBufferChangeFlag--;
		}
	}
}

void ShapesApp::UpdateMainPassConstantBuffer()
{
	// 렌더링 패스당 한번씩 갱신될 pass constant buffer의 갱신 함수

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

void ShapesApp::BuildConstantBufferViews()
{
	UINT objectCBByteSize = Helper::CalculateConstantBufferByteSize(sizeof(ObjectConstants));
	UINT objectCount = (UINT)m_opaqueRenderItems.size();

	for (int frameIndex = 0; frameIndex < NumFrameResources; ++frameIndex)
	{
		auto objectCB = m_frameResources[frameIndex]->objectConstantBuffer->Resource();
		for (UINT i = 0; i < objectCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();
			cbAddress += i * objectCBByteSize;

			int heapIndex = frameIndex * objectCount + i;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, m_cbvsrvDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objectCBByteSize;

			m_device->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

	UINT passCBByteSize = Helper::CalculateConstantBufferByteSize(sizeof(PassConstants));

	for (int frameIndex = 0; frameIndex < NumFrameResources; ++frameIndex)
	{
		auto passCB = m_frameResources[frameIndex]->passConstantBuffer->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

		int heapIndex = m_passCBVOffset + frameIndex;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, m_cbvsrvDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = passCBByteSize;

		m_device->CreateConstantBufferView(&cbvDesc, handle);
	}
}

void ShapesApp::BuildDescriptorHeaps()
{
	UINT objectCount = (UINT)m_opaqueRenderItems.size();
	UINT numDescriptors = (objectCount + 1) * NumFrameResources;

	m_passCBVOffset = objectCount * NumFrameResources;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;

	ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));
}

void ShapesApp::ConfigureRootSignature()
{
	// 2개의 Constant buffer가 resource로 들어갈 예정
	CD3DX12_DESCRIPTOR_RANGE cbvTable0;
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // constant buffer slot register 0

	CD3DX12_DESCRIPTOR_RANGE cbvTable1;
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1); // constant buffer slot register 1

	// root parameter을 descriptor table로 설정
	CD3DX12_ROOT_PARAMETER slotRootParameters[2] = {};
	slotRootParameters[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameters[1].InitAsDescriptorTable(1, &cbvTable1);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(2, slotRootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializeRootSignature = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializeRootSignature.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);
	ThrowIfFailed(m_device->CreateRootSignature(
		0, serializeRootSignature->GetBufferPointer(), serializeRootSignature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf())
	));
}

void ShapesApp::BuildShadersAndInputLayout()
{
	m_shaders["VS"] = Helper::CompileShader(L"ShapesVS.hlsl", nullptr, "main", "vs_5_1");
	m_shaders["PS"] = Helper::CompileShader(L"ShapesPS.hlsl", nullptr, "main", "ps_5_1");
	m_inputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA}
	};
}

void ShapesApp::BuildShapeGeometry()
{
	GeometryGenerator geoGenerator;

	GeometryGenerator::MeshData box = geoGenerator.CreateBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData grid = geoGenerator.CreateGrid(20.0f, 30.0f, 60, 40);
	//GeometryGenerator::MeshData sphere = geoGenerator.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData sphere = geoGenerator.CreateGeoSphere(0.5f, 3);
	GeometryGenerator::MeshData cylinder = geoGenerator.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

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

void ShapesApp::BuildRenderItems()
{
	auto boxRenderItem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRenderItem->inWorldSpace, XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRenderItem->objectConstantBufferIndex = 0;
	boxRenderItem->geo = m_geometries["ShapeGeo"].get();
	boxRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRenderItem->indexCount = boxRenderItem->geo->drawArgs["box"].IndexCount;
	boxRenderItem->startIndexLocation = boxRenderItem->geo->drawArgs["box"].StartIndexLocation;
	boxRenderItem->baseVertexLocation = boxRenderItem->geo->drawArgs["box"].BaseVertexLocation;

	m_allRenderItems.push_back(std::move(boxRenderItem));

	auto gridRenderItem = std::make_unique<RenderItem>();
	gridRenderItem->inWorldSpace = JRMath::Identity4x4();
	gridRenderItem->objectConstantBufferIndex = 1;
	gridRenderItem->geo = m_geometries["ShapeGeo"].get();
	gridRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRenderItem->indexCount = gridRenderItem->geo->drawArgs["grid"].IndexCount;
	gridRenderItem->startIndexLocation = gridRenderItem->geo->drawArgs["grid"].StartIndexLocation;
	gridRenderItem->baseVertexLocation = gridRenderItem->geo->drawArgs["grid"].BaseVertexLocation;

	m_allRenderItems.push_back(std::move(gridRenderItem));

	UINT objectCBIndex = 2;

	for (int i = 0; i < 5; ++i)
	{
		auto leftCylinderRenderItem = std::make_unique<RenderItem>();
		auto rightCylinderRenderItem = std::make_unique<RenderItem>();
		auto leftSphereRenderItem = std::make_unique<RenderItem>();
		auto rightSphereRenderItem = std::make_unique<RenderItem>();

		XMMATRIX leftCylinderWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		XMMATRIX rightCylinderWorld = XMMatrixTranslation(5.0f, 1.5f, -10.0f + i * 5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&leftCylinderRenderItem->inWorldSpace, leftCylinderWorld);
		leftCylinderRenderItem->objectConstantBufferIndex = objectCBIndex++;
		leftCylinderRenderItem->geo = m_geometries["ShapeGeo"].get();
		leftCylinderRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylinderRenderItem->indexCount = leftCylinderRenderItem->geo->drawArgs["cylinder"].IndexCount;
		leftCylinderRenderItem->startIndexLocation = leftCylinderRenderItem->geo->drawArgs["cylinder"].StartIndexLocation;
		leftCylinderRenderItem->baseVertexLocation = leftCylinderRenderItem->geo->drawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylinderRenderItem->inWorldSpace, rightCylinderWorld);
		rightCylinderRenderItem->objectConstantBufferIndex = objectCBIndex++;
		rightCylinderRenderItem->geo = m_geometries["ShapeGeo"].get();
		rightCylinderRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylinderRenderItem->indexCount = rightCylinderRenderItem->geo->drawArgs["cylinder"].IndexCount;
		rightCylinderRenderItem->startIndexLocation = rightCylinderRenderItem->geo->drawArgs["cylinder"].StartIndexLocation;
		rightCylinderRenderItem->baseVertexLocation = rightCylinderRenderItem->geo->drawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRenderItem->inWorldSpace, leftSphereWorld);
		leftSphereRenderItem->objectConstantBufferIndex = objectCBIndex++;
		leftSphereRenderItem->geo = m_geometries["ShapeGeo"].get();
		leftSphereRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRenderItem->indexCount = leftSphereRenderItem->geo->drawArgs["sphere"].IndexCount;
		leftSphereRenderItem->startIndexLocation = leftSphereRenderItem->geo->drawArgs["sphere"].StartIndexLocation;
		leftSphereRenderItem->baseVertexLocation = leftSphereRenderItem->geo->drawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRenderItem->inWorldSpace, rightSphereWorld);
		rightSphereRenderItem->objectConstantBufferIndex = objectCBIndex++;
		rightSphereRenderItem->geo = m_geometries["ShapeGeo"].get();
		rightSphereRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRenderItem->indexCount = rightSphereRenderItem->geo->drawArgs["sphere"].IndexCount;
		rightSphereRenderItem->startIndexLocation = rightSphereRenderItem->geo->drawArgs["sphere"].StartIndexLocation;
		rightSphereRenderItem->baseVertexLocation = rightSphereRenderItem->geo->drawArgs["sphere"].BaseVertexLocation;

		m_allRenderItems.push_back(std::move(leftCylinderRenderItem));
		m_allRenderItems.push_back(std::move(rightCylinderRenderItem));
		m_allRenderItems.push_back(std::move(leftSphereRenderItem));
		m_allRenderItems.push_back(std::move(rightSphereRenderItem));
	}

	for (auto& element : m_allRenderItems)
	{
		m_opaqueRenderItems.push_back(element.get());
	}
}

void ShapesApp::DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems)
{
	UINT objectCBByteSize = Helper::CalculateConstantBufferByteSize(sizeof(ObjectConstants));
	auto objectConstantBuffer = m_currentFrameResource->objectConstantBuffer->Resource();

	for (size_t i = 0; i < renderItems.size(); ++i)
	{
		auto renderItem = renderItems[i];

		D3D12_VERTEX_BUFFER_VIEW vbv[2] =
		{
			renderItem->geo->VertexPositionBufferView(),
			renderItem->geo->VertexColorBufferView()
		};

		commandList->IASetVertexBuffers(0, 2, vbv);
		commandList->IASetIndexBuffer(&renderItem->geo->IndexBufferView());
		commandList->IASetPrimitiveTopology(renderItem->primitiveType);

		UINT cbvIndex = m_currentFrameResourceIndex * (UINT)m_opaqueRenderItems.size() + renderItem->objectConstantBufferIndex;
		auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
		cbvHandle.Offset(cbvIndex, m_cbvsrvDescriptorSize);

		commandList->SetGraphicsRootDescriptorTable(0, cbvHandle);
		commandList->DrawIndexedInstanced(renderItem->indexCount, 1, renderItem->startIndexLocation, renderItem->baseVertexLocation, 0);
	}
}

void ShapesApp::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePSODesc;

	ZeroMemory(&opaquePSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePSODesc.InputLayout = { m_inputLayout.data(), (UINT)m_inputLayout.size() };
	opaquePSODesc.pRootSignature = m_rootSignature.Get();
	opaquePSODesc.VS =
	{
		reinterpret_cast<BYTE*>(m_shaders["VS"]->GetBufferPointer()),
		m_shaders["VS"]->GetBufferSize()
	};
	opaquePSODesc.PS =
	{
		reinterpret_cast<BYTE*>(m_shaders["PS"]->GetBufferPointer()),
		m_shaders["PS"]->GetBufferSize()
	};

	opaquePSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	opaquePSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePSODesc.SampleMask = UINT_MAX;
	opaquePSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePSODesc.NumRenderTargets = 1;
	opaquePSODesc.RTVFormats[0] = m_backBufferFormat;
	opaquePSODesc.SampleDesc.Count = m_4xMSAAState ? 4 : 1;
	opaquePSODesc.SampleDesc.Quality = m_4xMSAAState ? (m_4xMSAAQuality - 1) : 0;
	opaquePSODesc.DSVFormat = m_depthStencilFormat;

	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&opaquePSODesc, IID_PPV_ARGS(&m_PSOs["opaque"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePSODesc = opaquePSODesc;
	opaqueWireframePSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&opaqueWireframePSODesc, IID_PPV_ARGS(&m_PSOs["opaque_wireframe"])));
}

void ShapesApp::CameraRotateLeft()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixRotationY(0.1f);
	XMStoreFloat4x4(&m_view, view);
}

void ShapesApp::CameraRotateRight()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixRotationY(-0.1f);
	XMStoreFloat4x4(&m_view, view);
}

void ShapesApp::CameraRotateUp()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixRotationX(0.1f);
	XMStoreFloat4x4(&m_view, view);
}

void ShapesApp::CameraRotateDown()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixRotationX(-0.1f);
	XMStoreFloat4x4(&m_view, view);
}

void ShapesApp::MoveForward() 
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixTranslation(0.0f, 0.0f, -1.0f);
	XMStoreFloat4x4(&m_view, view);
}

void ShapesApp::MoveBackward() 
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixTranslation(0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&m_view, view);
}

void ShapesApp::MoveLeft() 
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixTranslation(1.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&m_view, view);
}

void ShapesApp::MoveRight() 
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixTranslation(-1.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&m_view, view);
}