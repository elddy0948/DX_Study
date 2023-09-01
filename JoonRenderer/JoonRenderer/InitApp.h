#pragma once

#include <DirectXColors.h>

#include "BaseApp.h"

class InitApp : public BaseApp
{
public:
	InitApp(HINSTANCE hInstance);
	~InitApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update() override;
	virtual void Draw() override;
};

InitApp::InitApp(HINSTANCE hInstance) : BaseApp(hInstance)
{
}

InitApp::~InitApp()
{
}

bool InitApp::Initialize()
{
	if (!BaseApp::Initialize())
		return false;
	return true;
}

void InitApp::OnResize()
{
	BaseApp::OnResize();
}

void InitApp::Update()
{

}

void InitApp::Draw()
{
	ThrowIfFailed(m_commandAllocator->Reset());
	ThrowIfFailed(m_commandList->Reset(
		m_commandAllocator.Get(),
		nullptr
	));
	m_commandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));
	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	m_commandList->ClearRenderTargetView(
		CurrentBackBufferView(),
		DirectX::Colors::LightSteelBlue,
		0,
		nullptr
	);

	m_commandList->ClearDepthStencilView(
		DepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f,
		0,
		0,
		nullptr
	);

	m_commandList->OMSetRenderTargets(
		1,
		&CurrentBackBufferView(),
		true,
		&DepthStencilView());

	m_commandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_commandList->Close());

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	ThrowIfFailed(m_swapChain->Present(0, 0));
	m_currentBackBuffer = (m_currentBackBuffer + 1) % SwapChainBufferCount;
	FlushCommandQueue();
}