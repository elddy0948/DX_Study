#include <Windows.h>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
)
{
	//Create Windows
	WNDCLASSEX wc{ 0 };

	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hInstance;
	wc.hCursor = NULL;
	wc.hbrBackground = HBRUSH(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"JoonsEngine";
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	// Windows Instance
	HWND hWnd;

	hWnd = CreateWindowExA(
		0, "JoonsEngine",
		"Joons Engine Window",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		200, 200,
		600, 400,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, SW_SHOW);

	MSG msg;

	while (GetMessage(&msg, hWnd, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}