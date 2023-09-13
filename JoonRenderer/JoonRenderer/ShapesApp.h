#pragma once

#include "Helper.h"
#include "BaseApp.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "GeometryGenerator.h"

using namespace Microsoft::WRL;
using namespace DirectX;

struct VertexPositionData
{
	XMFLOAT3 position;
};

struct VertexColorData
{
	XMFLOAT4 color;
};

class ShapesApp : public BaseApp
{
public:
	ShapesApp(HINSTANCE hInstance);
	ShapesApp(const ShapesApp& rhs) = delete;
	ShapesApp& operator=(const ShapesApp& rhs) = delete;
	~ShapesApp();

protected:
	void BuildFrameResources();

	// Constant Buffer ����
	void UpdateObjectConstantBuffers();
	void UpdateMainPassConstantBuffer();

	void ConfigureRootSignature();

	void BuildShapeGeometry();
	void BuildRenderItems();

	virtual void Update() override;
	virtual void Draw() override;

protected:
	// For frame resource
	static const int NumFrameResources = 3;
	std::vector<std::unique_ptr<FrameResource>> m_frameResources;
	FrameResource* m_currentFrameResource = nullptr;
	int m_currentFrameResourceIndex = 0;

	std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;

	// render items per pipeline state objects
	std::vector<RenderItem*> m_opaqueRenderItems;
	std::vector<RenderItem*> m_transparentRenderItems;

	PassConstants m_mainPassConstantBuffer;

	XMFLOAT4X4 m_view = Identity4x4;
	XMFLOAT4X4 m_proj = Identity4x4;
	XMFLOAT3 m_eyePos;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> m_shaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> m_PSOs;
};