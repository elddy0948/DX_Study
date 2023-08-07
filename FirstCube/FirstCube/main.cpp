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
	catch (std::exception e)
	{

		return 0;
	}
}