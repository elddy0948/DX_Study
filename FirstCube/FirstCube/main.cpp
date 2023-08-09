#include <iostream>
#include "DrawApp.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	try
	{
		DrawApp da(hInstance);

		if (!da.Initialize())
		{
			return 0;
		}
		return da.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}