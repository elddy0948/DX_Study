#include "CommandObjects.h"
#include "Device.h"

CommandObjects::CommandObjects(Device* device) : device(device) {
	m_CommandQueue = nullptr;
	m_CommandAllocator = nullptr;
	m_CommandList = nullptr;
}

CommandObjects::~CommandObjects() {
	if (m_CommandList) m_CommandList->Release();
	if (m_CommandAllocator) m_CommandAllocator->Release();
	if (m_CommandQueue) m_CommandQueue->Release();
}

void CommandObjects::Initialize() {
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	HRESULT hr;

	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hr = device->GetGPU()->CreateCommandQueue(&queueDesc, __uuidof(m_CommandQueue), (void**)&m_CommandQueue);
	assert(hr == S_OK);

	hr = device->GetGPU()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(m_CommandAllocator), (void**)&m_CommandAllocator);
	assert(hr == S_OK);

	hr = device->GetGPU()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator, nullptr, __uuidof(m_CommandList), (void**)&m_CommandList);
	assert(hr == S_OK);

	m_CommandList->Close();
}