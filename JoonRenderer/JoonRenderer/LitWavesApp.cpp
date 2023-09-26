#include "LitWavesApp.h"

using namespace DirectX;
using namespace Microsoft::WRL;

// FrameResource
FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT wavesVertexCount, UINT materialCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocator.GetAddressOf())));
	ObjectConstantsBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
	PassConstantsBuffer = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	VertexBuffer = std::make_unique<UploadBuffer<Vertex>>(device, wavesVertexCount, false);
	MaterialConstantsBuffer = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
}

FrameResource::~FrameResource() {}

// LitWavesApp
LitWavesApp::LitWavesApp(HINSTANCE hInstance) : BaseApp(hInstance) {}

LitWavesApp::~LitWavesApp()
{
	BaseApp::~BaseApp();
}

bool LitWavesApp::Initialize()
{
	if (!BaseApp::Initialize())
		return false;

	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	m_cbvsrvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_waves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildLandGeometry();
	BuildWavesGeometry();
	BuildMaterials();
	BuildRenderItems();
	BuildFrameResource();
	BuildPSOs();

	ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };

	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	return true;
}

void LitWavesApp::OnResize()
{
	BaseApp::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * 3.141592f, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_proj, P);
}

void LitWavesApp::Update() 
{
	m_currentFrameResourceIndex = (m_currentFrameResourceIndex + 1) % NumFrameResource;
	m_currentFrameResource = m_frameResources[m_currentFrameResourceIndex].get();

	if (m_currentFrameResource->fence != 0 && m_fence->GetCompletedValue() < m_currentFrameResource->fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_currentFrameResource->fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateObjectConstantBuffer();
	UpdateMaterialConstantBuffers();
	UpdateMainPassConstantBuffer();
	UpdateWaves();
}

void LitWavesApp::Draw() 
{
	auto cmdListAlloc = m_currentFrameResource->CommandAllocator;

	ThrowIfFailed(cmdListAlloc->Reset());

	ThrowIfFailed(m_commandList->Reset(cmdListAlloc.Get(), m_psos["opaque"].Get()));

	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	m_commandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	m_commandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_commandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	auto passCB = m_currentFrameResource->PassConstantsBuffer->Resource();
	m_commandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	DrawRenderItems(m_commandList.Get(), m_opaqueRenderItems);

	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
		CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT
	));

	ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(m_swapChain->Present(0, 0));

	m_currentBackBuffer = (m_currentBackBuffer + 1) % SwapChainBufferCount;

	m_currentFrameResource->fence = ++m_currentFence;

	m_commandQueue->Signal(m_fence.Get(), m_currentFence);
}

void LitWavesApp::UpdateObjectConstantBuffer()
{
	auto currentObjectCB = m_currentFrameResource->ObjectConstantsBuffer.get();

	for (auto& e : m_allRenderItems)
	{
		if (e->ConstantsBufferChangedFlag > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);
			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.world, XMMatrixTranspose(world));
			currentObjectCB->CopyData(e->ObjectConstantsBufferIndex, objConstants);

			e->ConstantsBufferChangedFlag--;
		}
	}
}

void LitWavesApp::UpdateMainPassConstantBuffer()
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

	auto currentPassCB = m_currentFrameResource->PassConstantsBuffer.get();
	currentPassCB->CopyData(0, m_mainPassCB);
}

void LitWavesApp::UpdateMaterialConstantBuffers()
{
	auto currentMaterialCB = m_currentFrameResource->MaterialConstantsBuffer.get();

	for (auto& material : m_materials)
	{
		Material* mat = material.second.get();

		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX materialTransform = XMLoadFloat4x4(&mat->MaterialTransform);

			MaterialConstants materialConstants;
			materialConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			materialConstants.FresnelR0 = mat->FresnelR0;
			materialConstants.Roughness = mat->Roughness;

			currentMaterialCB->CopyData(mat->ConstantBufferIndex, materialConstants);

			mat->NumFramesDirty--;
		}
	}
}

void LitWavesApp::UpdateWaves()
{
	static float t_base = 0.0f;

	if ((Win32Application::GetTimer().GameTime() - t_base) >= 0.25f)
	{
		int i = Rand(4, m_waves->RowCount() - 5);
		int j = Rand(4, m_waves->ColumnCount() - 5);

		float r = RandF(0.2f, 0.5f);

		m_waves->Disturb(i, j, r);
	}
	m_waves->Update(Win32Application::GetTimer().DeltaTime());

	auto currentWavesVB = m_currentFrameResource->VertexBuffer.get();

	for (int i = 0; i < m_waves->VertexCount(); ++i)
	{
		Vertex v;

		v.Position = m_waves->Position(i);
		v.Normal = m_waves->Normal(i);

		currentWavesVB->CopyData(i, v);
	}

	m_wavesRenderItem->Geo->VertexBufferGPU = currentWavesVB->Resource();
}

void LitWavesApp::BuildFrameResource()
{
	for (int i = 0; i < NumFrameResource; ++i)
	{
		m_frameResources.push_back(std::make_unique<FrameResource>(
			m_device.Get(), 1, (UINT)m_allRenderItems.size(), m_waves->VertexCount(), (UINT)m_materials.size()
		));
	}
}

void LitWavesApp::BuildMaterials()
{
	auto grass = std::make_unique<Material>();
	grass->Name = "grass";
	grass->ConstantBufferIndex = 0;
	grass->DiffuseAlbedo = XMFLOAT4(0.2f, 0.6f, 0.6f, 1.0f);
	grass->Roughness = 0.125f;

	auto water = std::make_unique<Material>();
	water->Name = "water";
	water->ConstantBufferIndex = 1;
	water->DiffuseAlbedo = XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
	water->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	water->Roughness = 0.0f;

	m_materials["grass"] = std::move(grass);
	m_materials["water"] = std::move(water);
}

void LitWavesApp::BuildRenderItems()
{
	auto waveRenderItem = std::make_unique<RenderItem>();
	waveRenderItem->World = JRMath::Identity4x4();
	waveRenderItem->ObjectConstantsBufferIndex = 0;
	waveRenderItem->Geo = m_geometries["wave_geo"].get();
	waveRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	waveRenderItem->IndexCount = waveRenderItem->Geo->DrawArgs["grid"].IndexCount;
	waveRenderItem->StartIndexLocation = waveRenderItem->Geo->DrawArgs["grid"].StartIndexLocation;
	waveRenderItem->BaseVertexLocation = waveRenderItem->Geo->DrawArgs["grid"].BaseVertexLocation;

	m_wavesRenderItem = waveRenderItem.get();
	m_opaqueRenderItems.push_back(waveRenderItem.get());
	m_allRenderItems.push_back(std::move(waveRenderItem));

	auto landRenderItem = std::make_unique<RenderItem>();
	landRenderItem->World = JRMath::Identity4x4();
	landRenderItem->ObjectConstantsBufferIndex = 0;
	landRenderItem->Geo = m_geometries["LandGeo"].get();
	landRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	landRenderItem->IndexCount = landRenderItem->Geo->DrawArgs["LandGrid"].IndexCount;
	landRenderItem->StartIndexLocation = landRenderItem->Geo->DrawArgs["LandGrid"].StartIndexLocation;
	landRenderItem->BaseVertexLocation = landRenderItem->Geo->DrawArgs["LandGrid"].BaseVertexLocation;

	m_opaqueRenderItems.push_back(landRenderItem.get());
	m_allRenderItems.push_back(std::move(landRenderItem));
}

void LitWavesApp::BuildRootSignature()
{
	CD3DX12_ROOT_PARAMETER rootParameterSlots[3] = {};
	rootParameterSlots[0].InitAsConstantBufferView(0); // object
	rootParameterSlots[1].InitAsConstantBufferView(1); // material
	rootParameterSlots[2].InitAsConstantBufferView(2); // pass

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(3, rootParameterSlots, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializeRootSignature = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, serializeRootSignature.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	ThrowIfFailed(hr);
	ThrowIfFailed(m_device->CreateRootSignature(0, serializeRootSignature->GetBufferPointer(), serializeRootSignature->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.GetAddressOf())));
}

void LitWavesApp::BuildShadersAndInputLayout()
{
	m_shaders["VS"] = Helper::CompileShader(L"LitWavesVS.hlsl", nullptr, "main", "vs_5_0");
	m_shaders["PS"] = Helper::CompileShader(L"LitWavesPS.hlsl", nullptr, "main", "ps_5_0");

	m_inputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
}

void LitWavesApp::BuildLandGeometry()
{
	GeometryGenerator geoGen;

	GeometryGenerator::MeshData grid = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);

	std::vector<Vertex> vertices(grid.vertices.size());

	for (size_t i = 0; i < grid.vertices.size(); ++i)
	{
		auto& p = grid.vertices[i].position;

		vertices[i].Position = p;
		vertices[i].Position.y = GetHeight(p.x, p.z);
		vertices[i].Normal = GetHillsNormal(p.x, p.z);
	}

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	
	std::vector<std::uint16_t> indices = grid.GetIndices16();

	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<LitMeshGeometry>();
	geo->Name = "LandGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = Helper::CreateDefaultBuffer(m_device.Get(), m_commandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->IndexBufferGPU = Helper::CreateDefaultBuffer(m_device.Get(), m_commandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexBufferFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;
	geo->DrawArgs = std::unordered_map<std::string, LitSubmeshGeometry>();

	LitSubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["LandGrid"] = submesh;

	m_geometries["LandGeo"] = std::move(geo);
}

void LitWavesApp::BuildWavesGeometry()
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

	UINT vbByteSize = m_waves->VertexCount() * sizeof(Vertex);
	UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<LitMeshGeometry>();
	geo->Name = "wave_geo";

	geo->VertexBufferCPU = nullptr;
	geo->IndexBufferCPU = nullptr;

	geo->VertexBufferGPU = nullptr;
	geo->IndexBufferGPU = nullptr;


	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->IndexBufferGPU = Helper::CreateDefaultBuffer(m_device.Get(), m_commandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexBufferFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	LitSubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["grid"] = submesh;

	m_geometries["wave_geo"] = std::move(geo);
}

void LitWavesApp::DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems)
{
	UINT objectCBByteSize = Helper::CalculateConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = Helper::CalculateConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = m_currentFrameResource->ObjectConstantsBuffer->Resource();
	auto matCB = m_currentFrameResource->MaterialConstantsBuffer->Resource();

	for (size_t i = 0; i < renderItems.size(); ++i)
	{
		auto renderItem = renderItems[i];

		commandList->IASetVertexBuffers(0, 1, &renderItem->Geo->VertexBufferView());
		commandList->IASetIndexBuffer(&renderItem->Geo->IndexBufferView());
		commandList->IASetPrimitiveTopology(renderItem->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objectCBAddress = objectCB->GetGPUVirtualAddress() + renderItem->ObjectConstantsBufferIndex * objectCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + renderItem->Material->ConstantBufferIndex * matCBByteSize;

		commandList->SetGraphicsRootConstantBufferView(0, objectCBAddress);
		commandList->SetGraphicsRootConstantBufferView(1, matCBAddress);

		commandList->DrawIndexedInstanced(renderItem->IndexCount, 1, renderItem->StartIndexLocation, renderItem->BaseVertexLocation, 0);
	}
}

void LitWavesApp::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;

	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { m_inputLayout.data(), (UINT)m_inputLayout.size() };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = {
		reinterpret_cast<BYTE*>(m_shaders["VS"]->GetBufferPointer()),
		m_shaders["VS"]->GetBufferSize()
	};

	psoDesc.PS = {
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

	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_psos["opaque"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC wireframePSODesc = psoDesc;
	wireframePSODesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(&wireframePSODesc, IID_PPV_ARGS(&m_psos["wireframe_opaque"])));
}

XMFLOAT3 LitWavesApp::GetHillsNormal(float x, float z) const
{
	XMFLOAT3 n(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		-0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));
	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}

float LitWavesApp::GetHeight(float x, float z) const
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

int LitWavesApp::Rand(int a, int b) const
{
	return a + rand() % ((b - a) + 1);
}

float LitWavesApp::RandF() const
{
	return (float)(rand()) / (float)RAND_MAX;
}

float LitWavesApp::RandF(float a, float b) const
{
	return a + RandF() * (b - a);
}