#pragma once
#ifndef COMMAND_OBJECTS_H
#define COMMAND_OBJECTS_H

#include "Headers.h"

class Device;

class CommandObjects {
public:
	CommandObjects(Device* device);
	~CommandObjects();

public:
	void Initialize();

private:
	ID3D12CommandQueue* m_CommandQueue;
	ID3D12CommandAllocator* m_CommandAllocator;
	ID3D12GraphicsCommandList* m_CommandList;

	Device* device;
};


#endif /* !COMMAND_OBJECTS_H */
