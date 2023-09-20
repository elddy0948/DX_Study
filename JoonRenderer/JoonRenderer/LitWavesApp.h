#pragma once

#include "Helper.h"
#include "BaseApp.h"
#include "JRMath.h"
#include "Material.h"

class LitWavesApp : public BaseApp
{
public:
	LitWavesApp(HINSTANCE hInstance);
	LitWavesApp(const LitWavesApp& rhs) = delete;
	LitWavesApp& operator=(const LitWavesApp& rhs) = delete;
	~LitWavesApp();
	

private:
	void BuildMaterials();

private:
	std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
};