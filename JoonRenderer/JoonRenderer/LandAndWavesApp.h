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

private:
	virtual void Draw() override;
	virtual void Update() override;

	void ConfigureRootSignature();

	void BuildLandGeometry();
	void DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<LAWAppRenderItem*>& renderItems);

	void UpdateWaves();

	float GetHeight(float x, float z) const;
	int Rand(int a, int b);
	float RandF();
	float RandF(float a, float b);
	
private:

	LAWAppFrameResource* m_currentFrameResource = nullptr;
	LAWAppRenderItem* m_wavesRenderItem = nullptr;

	std::vector<LAWAppRenderItem*> m_opaqueRenderItems;
	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;

	std::unique_ptr<Waves> m_waves;
}; 