#include "DrawApp.h"

using namespace DirectX;

DrawApp::DrawApp(HINSTANCE hInstance) : BaseApp(hInstance)
{
}

DrawApp::~DrawApp()
{
}

bool DrawApp::Initialize()
{
	if (!BaseApp::Initialize())
		return false;

	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	BuildConstantBufferDescriptorHeap();
	BuildConstantBuffer();
	BuildRootSignature();
	BuildShaders();
	SetInputLayout();
	BuildGeo();
	BuildPSO();

	ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();

	return true;
}

void DrawApp::OnResize()
{
	BaseApp::OnResize();
	
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * 3.141592f, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_proj, P);
}

void DrawApp::Update()
{
	float x = m_radius * sinf(m_phi) * cosf(m_theta);
	float z = m_radius * sinf(m_phi) * sinf(m_theta);
	float y = m_radius * cosf(m_phi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_view, view);

	XMMATRIX world = XMLoadFloat4x4(&m_world);
	XMMATRIX proj = XMLoadFloat4x4(&m_proj);
	XMMATRIX worldViewProj = world * view * proj;
	float gameTime = Win32Application::GetTimer().GameTime();

	ObjectConstants objectConstants;
	XMStoreFloat4x4(&objectConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	objectConstants.gTime = gameTime;
	m_objectConstantBuffer->CopyData(0, objectConstants);
}

void DrawApp::Draw()
{
	ThrowIfFailed(m_commandAllocator->Reset());
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pso.Get()));

	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	m_commandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));
	m_commandList->ClearRenderTargetView(
		CurrentBackBufferView(),
		Colors::LightSteelBlue,
		0,
		nullptr);
	m_commandList->ClearDepthStencilView(
		DepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f,
		0,
		0,
		nullptr);
	m_commandList->OMSetRenderTargets(
		1,
		&CurrentBackBufferView(),
		true,
		&DepthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_constantBufferHeap.Get() };
	m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[] =
	{
		m_geo->VertexPositionBufferView(),
		m_geo->VertexColorBufferView()
	};

	m_commandList->IASetVertexBuffers(0, 2, vertexBufferViews);
	m_commandList->IASetIndexBuffer(&m_geo->IndexBufferView());
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

	m_commandList->SetGraphicsRootDescriptorTable(
		0,
		m_constantBufferHeap->GetGPUDescriptorHandleForHeapStart()
	);

	m_commandList->DrawIndexedInstanced(
		m_geo->drawArgs["box"].IndexCount,
		1,
		m_geo->drawArgs["box"].StartIndexLocation,
		m_geo->drawArgs["box"].BaseVertexLocation,
		0
	);

	m_commandList->DrawIndexedInstanced(
		m_geo->drawArgs["pyramid"].IndexCount,
		1,
		m_geo->drawArgs["pyramid"].StartIndexLocation,
		m_geo->drawArgs["pyramid"].BaseVertexLocation,
		0
	);

	m_commandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		)
	);

	ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(m_swapChain->Present(0, 0));
	m_currentBackBuffer = (m_currentBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
}

Microsoft::WRL::ComPtr<ID3D12Resource> DrawApp::CreateDefaultBuffer(
	ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList,
	const void* initData,
	UINT64 byteSize,
	Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
	using namespace Microsoft::WRL;

	ComPtr<ID3D12Resource> defaultBuffer;

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

	ThrowIfFailed(device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));
	
	D3D12_SUBRESOURCE_DATA subResourceData = { };
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;

	commandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST));

	UpdateSubresources<1>(
		commandList,
		defaultBuffer.Get(),
		uploadBuffer.Get(),
		0,
		0,
		1,
		&subResourceData);

	commandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ));

	return defaultBuffer;
}

void DrawApp::BuildConstantBufferDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;

	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;

	ThrowIfFailed(m_device->CreateDescriptorHeap(
		&cbvHeapDesc,
		IID_PPV_ARGS(&m_constantBufferHeap)));
}

void DrawApp::BuildConstantBuffer()
{
	UINT n = 2;
	int i = 0;

	m_objectConstantBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(
		m_device.Get( ),
		n,
		true);

	UINT objectConstantBufferByteSize = Helper::CalculateConstantBufferByteSize(sizeof(ObjectConstants));
	D3D12_GPU_VIRTUAL_ADDRESS constantBufferAddress = m_objectConstantBuffer->Resource()->GetGPUVirtualAddress();

	int boxConstantBufferIndex = i;
	constantBufferAddress += boxConstantBufferIndex * objectConstantBufferByteSize;
	
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constantBufferAddress;
	cbvDesc.SizeInBytes = Helper::CalculateConstantBufferByteSize(sizeof(ObjectConstants));

	m_device->CreateConstantBufferView(
		&cbvDesc,
		m_constantBufferHeap->GetCPUDescriptorHandleForHeapStart());
}

void DrawApp::BuildRootSignature()
{
	using namespace Microsoft::WRL;

	CD3DX12_ROOT_PARAMETER slotRootParameter[1] = {};

	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
		1, 
		slotRootParameter,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSignature = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSignature.GetAddressOf(),
		errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	ThrowIfFailed(hr);

	ThrowIfFailed(m_device->CreateRootSignature(
		0,
		serializedRootSignature->GetBufferPointer(),
		serializedRootSignature->GetBufferSize(),
		IID_PPV_ARGS(&m_rootSignature)));
}

void DrawApp::BuildShaders()
{
	m_vsByteCode = Helper::CompileShader(L"MyVS.hlsl", nullptr, "main", "vs_5_0");
	m_psByteCode = Helper::CompileShader(L"MyPS.hlsl", nullptr, "main", "ps_5_0");
}

void DrawApp::SetInputLayout()
{
	m_inputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
}

void DrawApp::BuildGeo()
{
	std::array<VPosData, 13> vPosArray =
	{
		// Box
		VPosData({XMFLOAT3(-0.5f, -0.5f, -0.5f)}), VPosData({XMFLOAT3(-0.5f, 0.5f, -0.5f)}),
		VPosData({XMFLOAT3(0.5f, 0.5f, -0.5f)}), VPosData({XMFLOAT3(0.5f, -0.5f, -0.5f)}),
		VPosData({XMFLOAT3(-0.5f, -0.5f, 0.5f)}), VPosData({XMFLOAT3(-0.5f, 0.5f, 0.5f)}),
		VPosData({XMFLOAT3(0.5f, 0.5f, 0.5f)}), VPosData({XMFLOAT3(0.5f, -0.5f, 0.5f)}),
		// Pyramid
		VPosData({XMFLOAT3{0.0f, 0.5f, -1.0f}}), VPosData({XMFLOAT3{-1.0f, 0.5f, 0.0f}}),
		VPosData({XMFLOAT3{1.0f, 0.5f, 0.0f}}), VPosData({XMFLOAT3{0.0f, 0.5f, 1.0f}}),
		VPosData({XMFLOAT3{0.0f, 1.0f, 0.0f}}),
	};

	std::array<VColorData, 13> vColorArray =
	{
		// Box
		VColorData({XMFLOAT4(Colors::Black)}), VColorData({XMFLOAT4(Colors::Black)}),
		VColorData({XMFLOAT4(Colors::Black)}), VColorData({XMFLOAT4(Colors::Black)}),
		VColorData({XMFLOAT4(Colors::Black)}), VColorData({XMFLOAT4(Colors::Black)}),
		VColorData({XMFLOAT4(Colors::Black)}), VColorData({XMFLOAT4(Colors::Black)}),
		// Pyramid
		VColorData({XMFLOAT4{Colors::Green}}), VColorData({XMFLOAT4{Colors::Green}}),
		VColorData({XMFLOAT4{Colors::Green}}), VColorData({XMFLOAT4{Colors::Green}}),
		VColorData({XMFLOAT4{Colors::Red}}),
	};

	std::array<std::uint16_t, 54> indices =
	{
		// Box
		// front
		0, 1, 2, 0, 2, 3,
		// back
		4, 6, 5, 4, 7, 6,
		// left
		4, 5, 1, 4, 1, 0,
		// right
		3, 2, 6, 3, 6, 7,
		// upper
		1, 5, 6, 1, 6, 2,
		// below
		4, 0, 3, 4, 3, 7,
		// Pyramid
		8, 10, 9,
		9, 10, 11,
		8, 9, 12,
		8, 12, 10,
		10, 12, 11,
		11, 12, 9,
	};

	const UINT vPosBufferByteSize = (UINT)vPosArray.size() * sizeof(VPosData);
	const UINT vColorBufferByteSize = (UINT)vColorArray.size() * sizeof(VColorData);
	const UINT indexBufferByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_geo = std::make_unique<MeshGeometry>();
	m_geo->Name = "Geometry";

	ThrowIfFailed(D3DCreateBlob(
		vPosBufferByteSize,
		&m_geo->vPosBufferCPU
	));

	ThrowIfFailed(D3DCreateBlob(
		vColorBufferByteSize,
		&m_geo->vColorBufferCPU
	));

	ThrowIfFailed(D3DCreateBlob(
		indexBufferByteSize,
		&m_geo->indexBufferCPU
	));

	CopyMemory(
		m_geo->vPosBufferCPU->GetBufferPointer(),
		vPosArray.data(),
		vPosBufferByteSize
	);

	CopyMemory(
		m_geo->vColorBufferCPU->GetBufferPointer(),
		vColorArray.data(),
		vColorBufferByteSize
	);

	CopyMemory(
		m_geo->indexBufferCPU->GetBufferPointer(),
		indices.data(),
		indexBufferByteSize
	);

	m_geo->vPosBufferGPU = CreateDefaultBuffer(
		m_device.Get(),
		m_commandList.Get(),
		vPosArray.data(),
		vPosBufferByteSize,
		m_geo->vPosBufferUploader
	);

	m_geo->vColorBufferGPU = CreateDefaultBuffer(
		m_device.Get(),
		m_commandList.Get(),
		vColorArray.data(),
		vColorBufferByteSize,
		m_geo->vColorBufferUploader
	);

	m_geo->indexBufferGPU = CreateDefaultBuffer(
		m_device.Get(),
		m_commandList.Get(),
		indices.data(),
		indexBufferByteSize,
		m_geo->indexBufferUploader
	);

	m_geo->vPosByteStride = sizeof(VPosData);
	m_geo->vColorByteStride = sizeof(VColorData);
	
	m_geo->indexFormat = DXGI_FORMAT_R16_UINT;

	m_geo->vPosBufferByteSize = vPosBufferByteSize;
	m_geo->vColorBufferByteSize = vColorBufferByteSize;
	m_geo->indexBufferByteSize = indexBufferByteSize;

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = 36;
	boxSubmesh.BaseVertexLocation = 0;
	boxSubmesh.StartIndexLocation = 0;

	m_geo->drawArgs["box"] = boxSubmesh;
	
	SubmeshGeometry pyramidSubmesh;
	pyramidSubmesh.IndexCount = 18;
	pyramidSubmesh.BaseVertexLocation = 8;
	pyramidSubmesh.StartIndexLocation = boxSubmesh.IndexCount;

	m_geo->drawArgs["pyramid"] = pyramidSubmesh;
}

void DrawApp::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_inputLayout.data(), (UINT)m_inputLayout.size() };
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize()
	};

	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize()
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
	
	ThrowIfFailed(m_device->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&m_pso)));
}

void DrawApp::OnLeftKeyDown()
{
	float dx = XMConvertToRadians(1.0f);
	m_theta += dx;
}

void DrawApp::OnRightKeyDown()
{
	float dx = XMConvertToRadians(1.0f);
	m_theta -= dx;
}

void DrawApp::OnUpKeyDown()
{
	float dy = XMConvertToRadians(1.0f);
	m_phi += dy;
}

void DrawApp::OnDownKeyDown()
{
	float dy = XMConvertToRadians(1.0f);
	m_phi -= dy;
}