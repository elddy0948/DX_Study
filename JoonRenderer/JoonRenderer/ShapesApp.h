#pragma once

#include "Helper.h"
#include "BaseApp.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "GeometryGenerator.h"
#include "JRMath.h"

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

	virtual bool Initialize() override;

private:
	virtual void Update() override;
	virtual void Draw() override;
	virtual void OnResize() override;

	void BuildFrameResources();

	// Constant Buffer °»½Å
	void UpdateObjectConstantBuffers();
	void UpdateMainPassConstantBuffer();

	void BuildConstantBufferViews();
	void BuildDescriptorHeaps();
	void ConfigureRootSignature();
	void BuildShadersAndInputLayout();

	void BuildShapeGeometry();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems);

	void BuildPSOs();

private:
	// Camera movement
	virtual void CameraRotateUp() override;
	virtual void CameraRotateDown() override;
	virtual void CameraRotateRight() override;
	virtual void CameraRotateLeft() override;

	virtual void MoveForward() override;
	virtual void MoveBackward() override;
	virtual void MoveLeft() override;
	virtual void MoveRight() override;

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

	XMFLOAT4X4 m_view = JRMath::Identity4x4();
	XMFLOAT4X4 m_proj = JRMath::Identity4x4();
	XMFLOAT3 m_eyePos = { 0.0f, 0.0f, 0.0f };

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> m_shaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> m_PSOs;
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

	ComPtr<ID3D12DescriptorHeap> m_cbvHeap = nullptr;
	ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

	UINT m_passCBVOffset = 0;

	bool m_IsWireFrame = true;

	float m_theta = 1.5f * XM_PI;
	float m_phi = XM_PIDIV4;
	float m_radius = 5.0f;
};