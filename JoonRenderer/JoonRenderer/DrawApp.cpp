#include "DrawApp.h"

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

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
	return true;
}

void DrawApp::OnResize()
{
	BaseApp::OnResize();
}

void DrawApp::Update()
{

}

void DrawApp::Draw()
{

}

void DrawApp::SetInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
}

void DrawApp::ConfigureVertexBuffer()
{
	Vertex vertices[] =
	{
		{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White)},
		{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Black)},
		{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(Colors::Red)},
		{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green)},
		{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Blue)},
		{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(Colors::Yellow)},
		{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(Colors::Cyan)},
		{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(Colors::Magenta)},
	};

	UINT64 vertexBufferByteSize = 8 * sizeof(Vertex);

	vertexBufferGPU = CreateDefaultBuffer(
		m_device.Get(),
		m_commandList.Get(),
		vertices,
		vertexBufferByteSize,
		vertexBufferUploader);

	// Vertex buffer view
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = sizeof(Vertex);
	vbv.SizeInBytes = 8 * sizeof(Vertex);

	D3D12_VERTEX_BUFFER_VIEW vertexBuffers[1] = { vbv };

	m_commandList->IASetVertexBuffers(0, 1, vertexBuffers);

	m_commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

void DrawApp::ConfigureIndexBuffer()
{
	std::uint16_t indices[] =
	{
		// front
		0, 1, 2,
		0, 2, 3,
		// back
		4, 6, 5,
		4, 7, 6,
		// left
		4, 5, 1,
		4, 1, 0,
		// right
		3, 2, 6,
		3, 6, 7,
		// upper
		1, 5, 6,
		1, 6, 2,
		// below
		4, 0, 3,
		4, 3, 7,
	};

	UINT indexBufferByteSize = 36 * sizeof(std::uint16_t);

	indexBufferGPU = CreateDefaultBuffer(
		m_device.Get(),
		m_commandList.Get(),
		indices,
		indexBufferByteSize,
		indexBufferUploader);

	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = DXGI_FORMAT_R16_UINT;
	ibv.SizeInBytes = indexBufferByteSize;

	m_commandList->IASetIndexBuffer(&ibv);
}

void DrawApp::ConfigureConstantBuffer()
{
	UINT elementByteSize = Helper::CalculateConstantBufferByteSize(sizeof(ObjectConstants));

	m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(elementByteSize * NumElements),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_uploadConstantBuffer));
}