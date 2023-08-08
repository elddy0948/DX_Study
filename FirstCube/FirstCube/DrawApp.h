#pragma once

#include <DirectXMath.h>
#include "D3DApp.h"

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 Tex0;
	DirectX::XMFLOAT2 Tex1;
};

struct Vertex2
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT4 Color;
};

class DrawApp : public D3DApp
{
public:
	DrawApp(HINSTANCE hInstance);
	~DrawApp();

	virtual bool Initialize() override;

	void SetupInputElements();
	void CreateVertices2();
protected:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

private:
	const UINT64 vbByteSize = 8 * sizeof(Vertex2);

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
};