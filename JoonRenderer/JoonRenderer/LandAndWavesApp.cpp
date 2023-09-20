#include "LandAndWavesApp.h"

LandAndWavesApp::LandAndWavesApp(HINSTANCE hInstance) : BaseApp(hInstance) {}

LandAndWavesApp::~LandAndWavesApp() { BaseApp::~BaseApp(); }

bool LandAndWavesApp::Initialize()
{
	if (!BaseApp::Initialize())
		return false;

	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
	m_waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	ConfigureRootSignature();
	BuildShadersAndInputLayout();
	BuildLandGeometry();
	BuildWavesGeometry();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs();

	ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* cmdsLists[] = {m_commandList.Get()};
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();

	return true;
}

void LandAndWavesApp::OnResize()
{
	BaseApp::OnResize();

	XMMATRIX p = XMMatrixPerspectiveFovLH(0.25f * 3.141592f, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_proj, p);
}

void LandAndWavesApp::Update()
{
	m_currentFrameResourceIndex = (m_currentFrameResourceIndex + 1) % NumFrameResources;
	m_currentFrameResource = m_frameResources[m_currentFrameResourceIndex].get();

	if (m_currentFrameResource->fence != 0 && m_fence->GetCompletedValue() < m_currentFrameResource->fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_currentFrameResource->fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateObjectConstantBuffer();
	UpdateMainPassConstantBuffer();
	UpdateWaves();
}

void LandAndWavesApp::UpdateObjectConstantBuffer()
{
	auto currentObjectCB = m_currentFrameResource->objectConstantBuffer.get();
	for (auto& e : m_allRenderItems)
	{
		if(e->constantBufferChangeFlag > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->inWorldSpace);
			ObjectConstants objectConstants;
			XMStoreFloat4x4(&objectConstants.world, XMMatrixTranspose(world));
			currentObjectCB->CopyData(e->objectConstantBufferIndex, objectConstants);
			e->constantBufferChangeFlag--;
		}
	}
}

void LandAndWavesApp::UpdateMainPassConstantBuffer()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	XMMATRIX proj = XMLoadFloat4x4(&m_proj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&m_mainPassCB.view, XMMatrixTranspose(view));
	XMStoreFloat4x4(&m_mainPassCB.invView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&m_mainPassCB.proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&m_mainPassCB.invProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&m_mainPassCB.viewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&m_mainPassCB.invViewProj, XMMatrixTranspose(invViewProj));

	m_mainPassCB.eyePos = m_eyePosition;
	m_mainPassCB.renderTargetSize = XMFLOAT2((float)m_clientWidth, (float)m_clientHeight);
	m_mainPassCB.invRenderTargetSize = XMFLOAT2(1.0f / m_clientWidth, 1.0f / m_clientHeight);
	m_mainPassCB.nearZ = 1.0f;
	m_mainPassCB.farZ = 1000.0f;
	m_mainPassCB.totalTime = Win32Application::GetTimer().GameTime();
	m_mainPassCB.deltaTime = Win32Application::GetTimer().DeltaTime();

	auto currentPassCB = m_currentFrameResource->passConstantBuffer.get();
	currentPassCB->CopyData(0, m_mainPassCB);
}

void LandAndWavesApp::Draw()
{
	auto commandAllocator = m_currentFrameResource->commandAllocator;

	ThrowIfFailed(commandAllocator->Reset());

	if (m_isWireframe)
	{
		ThrowIfFailed(m_commandList->Reset(commandAllocator.Get(), m_pso["opaque_wireframe"].Get()));
	}
	else
	{
		ThrowIfFailed(m_commandList->Reset(commandAllocator.Get(), m_pso["opaque"].Get()));
	}

	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	
	m_commandList->ClearRenderTargetView(CurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
	m_commandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_commandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	auto passCB = m_currentFrameResource->passConstantBuffer->Resource();
	m_commandList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());
	DrawRenderItems(m_commandList.Get(), m_opaqueRenderItems);
	
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() }; 
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(m_swapChain->Present(0, 0));

	m_currentBackBuffer = (m_currentBackBuffer + 1) % SwapChainBufferCount;

	m_currentFrameResource->fence = ++m_currentFence;
	m_commandQueue->Signal(m_fence.Get(), m_currentFence);
}

void LandAndWavesApp::BuildFrameResources()
{
	for (int i = 0; i < NumFrameResources; ++i)
	{
		m_frameResources.push_back(std::make_unique<LAWAppFrameResource>(m_device.Get(), 1, (UINT)m_allRenderItems.size(), m_waves->VertexCount()));
	}
}

void LandAndWavesApp::BuildRenderItems()
{
	auto waveRenderItem = std::make_unique<LAWAppRenderItem>();
	waveRenderItem->inWorldSpace = JRMath::Identity4x4();
	waveRenderItem->objectConstantBufferIndex = 0;
	waveRenderItem->geo = m_geometries["WaveGeo"].get();
	waveRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	waveRenderItem->indexCount = waveRenderItem->geo->drawArgs["grid"].IndexCount;
	waveRenderItem->startIndexLocation = waveRenderItem->geo->drawArgs["grid"].StartIndexLocation;
	waveRenderItem->baseVertexLocation = waveRenderItem->geo->drawArgs["grid"].BaseVertexLocation;

	m_wavesRenderItem = waveRenderItem.get();
	m_opaqueRenderItems.push_back(waveRenderItem.get());
	m_allRenderItems.push_back(std::move(waveRenderItem));

	auto landRenderItem = std::make_unique<LAWAppRenderItem>();
	landRenderItem->inWorldSpace = JRMath::Identity4x4();
	landRenderItem->objectConstantBufferIndex = 1;
	landRenderItem->geo = m_geometries["LandGeo"].get();
	landRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	landRenderItem->indexCount = landRenderItem->geo->drawArgs["grid"].IndexCount;
	landRenderItem->startIndexLocation = landRenderItem->geo->drawArgs["grid"].StartIndexLocation;
	landRenderItem->baseVertexLocation = landRenderItem->geo->drawArgs["grid"].BaseVertexLocation;

	m_opaqueRenderItems.push_back(landRenderItem.get());
	m_allRenderItems.push_back(std::move(landRenderItem));
}

void LandAndWavesApp::ConfigureRootSignature()
{
	CD3DX12_ROOT_PARAMETER rootParameterSlots[2] = {};
	rootParameterSlots[0].InitAsConstantBufferView(0); // object cb
	rootParameterSlots[1].InitAsConstantBufferView(1); // pass cb

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(2, rootParameterSlots, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializeRootSignature = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializeRootSignature.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);
	ThrowIfFailed(m_device->CreateRootSignature(0, serializeRootSignature->GetBufferPointer(), serializeRootSignature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf())));
}

void LandAndWavesApp::BuildShadersAndInputLayout()
{
	m_shaders["VS"] = Helper::CompileShader(L"LAW_VS.hlsl", nullptr, "main", "vs_5_0");
	m_shaders["PS"] = Helper::CompileShader(L"LAW_PS.hlsl", nullptr, "main", "ps_5_0");
	m_inputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
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
		vertexPositions[i].position.y = GetHeight(p.x, p.z);

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
			vertexColors[i].color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if (vertexPositions[i].position.y < 20.0f)
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

void LandAndWavesApp::BuildWavesGeometry()
{
	std::vector<std::uint16_t> indices(3 * m_waves->TriangleCount());
	assert(m_waves->VertexCount() < 0x0000ffff);

	int m = m_waves->RowCount();
	int n = m_waves->ColumnCount();
	int k = 0;

	for (int i = 0; i < m - 1; ++i)
	{
		for (int j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}

	UINT vpbByteSize = m_waves->VertexCount() * sizeof(VertexPositionData);
	UINT vcbByteSize = m_waves->VertexCount() * sizeof(VertexColorData);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "WaveGeo";

	geo->vPosBufferCPU = nullptr;
	geo->vPosBufferGPU = nullptr;

	geo->vColorBufferCPU = nullptr;
	geo->vColorBufferGPU = nullptr;

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->indexBufferCPU));
	CopyMemory(geo->indexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->indexBufferGPU = Helper::CreateDefaultBuffer(m_device.Get(), m_commandList.Get(), indices.data(), ibByteSize, geo->indexBufferUploader);

	geo->vPosByteStride = sizeof(VertexPositionData);
	geo->vPosBufferByteSize = vpbByteSize;
	geo->vColorByteStride = sizeof(VertexColorData);
	geo->vColorBufferByteSize = vcbByteSize;

	geo->indexFormat = DXGI_FORMAT_R16_UINT;
	geo->indexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->drawArgs["grid"] = submesh;

	m_geometries["WaveGeo"] = std::move(geo);
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

void LandAndWavesApp::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;

	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { m_inputLayout.data(), (UINT)m_inputLayout.size() };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_shaders["VS"]->GetBufferPointer()),
		m_shaders["VS"]->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_shaders["PS"]->GetBufferPointer()),
		m_shaders["PS"]->GetBufferSize()
	};

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = m_backBufferFormat;
	psoDesc.SampleDesc.Count = m_4xMSAAState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m_4xMSAAState ? (m_4xMSAAQuality - 1) : 0;
	psoDesc.DSVFormat = m_depthStencilFormat;

	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso["opaque"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC wireframePSODesc = psoDesc;
	wireframePSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&wireframePSODesc, IID_PPV_ARGS(&m_pso["opaque_wireframe"])));
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

void LandAndWavesApp::CameraRotateLeft()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixRotationY(0.1f);
	XMStoreFloat4x4(&m_view, view);
}

void LandAndWavesApp::CameraRotateRight()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixRotationY(-0.1f);
	XMStoreFloat4x4(&m_view, view);
}

void LandAndWavesApp::CameraRotateUp()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixRotationX(0.1f);
	XMStoreFloat4x4(&m_view, view);
}

void LandAndWavesApp::CameraRotateDown()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixRotationX(-0.1f);
	XMStoreFloat4x4(&m_view, view);
}

void LandAndWavesApp::MoveForward()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixTranslation(0.0f, 0.0f, -1.0f);
	XMStoreFloat4x4(&m_view, view);
}

void LandAndWavesApp::MoveBackward()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixTranslation(0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&m_view, view);
}

void LandAndWavesApp::MoveLeft()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixTranslation(1.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&m_view, view);
}

void LandAndWavesApp::MoveRight()
{
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	view *= XMMatrixTranslation(-1.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&m_view, view);
}