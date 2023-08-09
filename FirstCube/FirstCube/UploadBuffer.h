#pragma once

#include "D3DHelper.h"

template <typename T>
class UploadBuffer
{
public:
	UploadBuffer(
		ID3D12Device* device,
		UINT elementCount,
		bool isConstantBuffer);

	UploadBuffer(const UploadBuffer& rhs) = delete;
	UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

	~UploadBuffer();

	ID3D12Resource* Resource() const;

	void CopyData(int elementIndex, const T& data);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
	BYTE* mMappedData = nullptr;

	UINT mElementByteSize = 0;
	bool mIsConstantBuffer = false;
};