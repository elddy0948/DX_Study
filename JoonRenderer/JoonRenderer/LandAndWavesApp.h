#pragma once

#include "BaseApp.h"
#include "Helper.h"
#include "GeometryGenerator.h"
#include "LAWAppRenderItem.h"
#include "Waves.h"

using namespace DirectX;

class LandAndWavesApp : public BaseApp
{
public:
	LandAndWavesApp(HINSTANCE hInstance);
	LandAndWavesApp(const LandAndWavesApp& rhs) = delete;
	LandAndWavesApp& operator=(const LandAndWavesApp& rhs) = delete;
	~LandAndWavesApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Draw() override;
	virtual void Update() override;

	void UpdateObjectConstantBuffer();
	void UpdateMainPassConstantBuffer();

	void BuildFrameResources();
	void BuildRenderItems();

	void ConfigureRootSignature();
	void BuildShadersAndInputLayout();

	void BuildLandGeometry();
	void BuildWavesGeometry();

	void DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<LAWAppRenderItem*>& renderItems);
	void UpdateWaves();

	void BuildPSOs();

	float GetHeight(float x, float z) const;
	int Rand(int a, int b);
	float RandF();
	float RandF(float a, float b);
	
private:
	std::vector<std::unique_ptr<LAWAppFrameResource>> m_frameResources;
	LAWAppFrameResource* m_currentFrameResource = nullptr;
	int m_currentFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;

	PassConstants m_mainPassCB;

	std::vector<std::unique_ptr<LAWAppRenderItem>> m_allRenderItems;
	std::vector<LAWAppRenderItem*> m_opaqueRenderItems;
	LAWAppRenderItem* m_wavesRenderItem = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> m_shaders;

	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> m_pso;

	bool m_isWireframe = false;

	std::unique_ptr<Waves> m_waves;

	XMFLOAT4X4 m_view = Identity4x4;
	XMFLOAT4X4 m_proj = Identity4x4;
	XMFLOAT3 m_eyePosition = { 0.0f, 0.0f, 0.0f };

	float m_theta = 1.5f * XM_PI;
	float m_phi = XM_PIDIV4;
	float m_radius = 5.0f;
}; 