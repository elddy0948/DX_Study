#include <iostream>
#include "BackgroundApp.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	try
	{
		BackgroundApp ba(hInstance);

		if (!ba.Initialize())
		{
			return 0;
		}

		return ba.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}