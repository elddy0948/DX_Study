#include "ShapesApp.h"

#include <d3dcommon.h>

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

// 렌더링 패스당 한번씩 갱신될 pass constant buffer의 갱신 함수
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
}

void ShapesApp::Draw()
{
	m_currentFrameResource->fence = ++m_currentFence;
	m_commandQueue->Signal(m_fence.Get(), m_currentFence);
}