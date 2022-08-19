//#include <Windows.h>
#include "MainWindow.h"

int CALLBACK wWinMain(
	HINSTANCE hInstance, HINSTANCE,
	PWSTR pCmdLine,
	int nCmdShow)
{
	base::MainWindow win;

	if (!win.Create(L"Hello Windows!", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	MSG msg = {};

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

