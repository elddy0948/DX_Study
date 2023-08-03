#include <iostream>
#include "D3DApp.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	D3DApp cubeApp(hInstance);

	if (!cubeApp.InitMainWindow())
	{
		MessageBox(nullptr, L"Init window failed.", 0, 0);
		return -1;
	}
	else
	{
		cubeApp.Run();
	}

}