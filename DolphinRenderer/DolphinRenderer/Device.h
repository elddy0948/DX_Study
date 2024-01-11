#pragma once
#ifndef DEVICE_H
#define DEVICE_H

#include "Headers.h"

#include "CommandObjects.h"

class Device {
public:
	Device();
	~Device();

public:
	bool initialize();
	ID3D12Device* GetGPU() { return mDevice; }

private:
	void GetDescriptorSize();
	void CreateFence();
	void CheckMultisampleQualityLevels();

private:
	IDXGIFactory* mFactory = nullptr;
	ID3D12Device* mDevice = nullptr;

#if defined(DEBUG) || defined(_DEBUG)
	void EnableDebugLayer();
	ID3D12Debug* debugController = nullptr;
#endif

	ID3D12Fence* mFence = nullptr;

	CommandObjects* m_CmdObjects;

	/* Descriptor sizes */
	UINT mRTVDescriptorSize = 0;
	UINT mDSVDescriptorSize = 0;
	UINT mCBVSRVDescriptorSize = 0;

	UINT m4xMSAAQuality = 0;
};

#endif /* !DEVICE_H */
