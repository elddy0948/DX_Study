#include "framework.h"
#include "InitD3D.h"
#include "d3dapp.h"

class InitDirect3DApp : public D3DApp
{
public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();

	virtual bool Initialize() override;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	InitDirect3DApp theApp(hInstance);
	if (!theApp.Initialize()) return 0;
	return theApp.Run();
}

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
	:D3DApp(hInstance)
{}

InitDirect3DApp::~InitDirect3DApp() {}

bool InitDirect3DApp::Initialize()
{
	if (!D3DApp::Initialize()) { return false; }
	return true;
}


