#include "Headers.h"
#include "TriangleApp.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
	TriangleApp app(800, 600, L"Triangle");

	return Win32App::Run(&app, hInstance, nCmdShow);
}