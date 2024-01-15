#pragma once
#ifndef TRIANGLE_APP_H
#define TRIANGLE_APP_H

#include "Headers.h"
#include "BaseApp.h"
#include "Win32App.h"

using namespace DirectX;

class TriangleApp : public BaseApp {
public:
	TriangleApp(UINT width, UINT height, std::wstring name);

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();

private:
	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void WaitForPreviousFrame();

private:
	static const UINT FrameCount = 2;

	struct Vertex {
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	IDXGISwapChain3* m_swapchain;
	ID3D12Device* m_device;
	ID3D12Resource* m_renderTargets[FrameCount];
	ID3D12CommandAllocator* m_commandAllocator;
	ID3D12CommandQueue* m_commandQueue;
	ID3D12RootSignature* m_rootSignature;
	ID3D12DescriptorHeap* m_rtvHeap;
	ID3D12PipelineState* m_pipelineState;
	ID3D12GraphicsCommandList* m_commandList;
	
	UINT m_rtvDescriptorSize;

	ID3D12Resource* m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	UINT m_frameIndex;
	HANDLE m_fenceEvent;
	ID3D12Fence* m_fence;
	UINT64 m_fenceValue;
};


#endif /* !TRIANGLE_APP_H */
