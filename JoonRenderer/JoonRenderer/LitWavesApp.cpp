#include "LitWavesApp.h"

LitWavesApp::LitWavesApp(HINSTANCE hInstance) : BaseApp(hInstance) {}

LitWavesApp::~LitWavesApp()
{
	BaseApp::~BaseApp();
}

void LitWavesApp::BuildMaterials()
{
	auto grass = std::make_unique<Material>();
	grass->Name = "grass";
	grass->ConstantBufferIndex = 0;
	grass->DiffuseAlbedo = XMFLOAT4(0.2f, 0.6f, 0.6f, 1.0f);
	grass->Roughness = 0.125f;

	auto water = std::make_unique<Material>();
	water->Name = "water";
	water->ConstantBufferIndex = 1;
	water->DiffuseAlbedo = XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
	water->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	water->Roughness = 0.0f;

	m_materials["grass"] = std::move(grass);
	m_materials["water"] = std::move(water);
}