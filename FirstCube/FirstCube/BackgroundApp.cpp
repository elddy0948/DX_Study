#include "BackgroundApp.h"
#include <DirectXColors.h>

BackgroundApp::BackgroundApp(HINSTANCE hInstance)
	: D3DApp(hInstance) {}

BackgroundApp::~BackgroundApp() {}

bool BackgroundApp::Initialize()
{
	if (!D3DApp::Initialize())
		return false;
	return true;
}

void BackgroundApp::OnResize()
{
	D3DApp::OnResize();
}

void BackgroundApp::Update(const GameTimer& gt)
{
}

void BackgroundApp::Draw(const GameTimer& gt)
{
	ThrowIfFailed(mDirectCmdListAllocator->Reset());
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAllocator.Get(), nullptr));

	mCommandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	mCommandList->ClearRenderTargetView(
		CurrentBackBufferView(),
		DirectX::Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(
		DepthStencilView(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	mCommandList->ResourceBarrier(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition(
			CurrentBackBuffer(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(mCommandList->Close());

	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrentBackBuffer = (mCurrentBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
}