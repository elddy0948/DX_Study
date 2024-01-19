#include "Win32App.h"

HWND Win32App::m_hwnd = nullptr;
GameTimer Win32App::m_timer = GameTimer();
bool Win32App::m_appPaused = false;
std::wstring Win32App::m_appTitle = L"";

int Win32App::Run(BaseApp* pApp, HINSTANCE hInstance, int nCmdShow) {
	int argc;

	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	pApp->ParseCommandLineArgs(argv, argc);
	LocalFree(argv);

	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = L"DXSampleClass";
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, static_cast<LONG>(pApp->GetWidth()), static_cast<LONG>(pApp->GetHeight()) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	m_hwnd = CreateWindow(
		windowClass.lpszClassName,
		pApp->GetTitle(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInstance,
		pApp);

	pApp->OnInit();

	ShowWindow(m_hwnd, nCmdShow);

	Win32App::m_timer.Reset();
	Win32App::m_appTitle = pApp->GetTitle();

	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			Win32App::m_timer.Tick();

			if (!Win32App::m_appPaused)
			{
				Win32App::CalculateFrameStats();
				pApp->OnUpdate();
				pApp->OnRender();
			}
		}
	}

	pApp->OnDestroy();

	return static_cast<char>(msg.wParam);
}

void Win32App::CalculateFrameStats()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;
	frameCount++;

	if (Win32App::m_timer.GameTime() - timeElapsed >= 1.0f)
	{
		float fps = static_cast<float>(frameCount);
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = Win32App::m_appTitle +
			L" fps : " + fpsStr +
			L" mspf : " + mspfStr;

		SetWindowText(Win32App::m_hwnd, windowText.c_str());

		frameCount = 0;
		timeElapsed += 1.0f;
	}
}

LRESULT Win32App::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	BaseApp* pApp = reinterpret_cast<BaseApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message) {
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			Win32App::m_appPaused = true;
			Win32App::m_timer.Stop();
		}
		else
		{
			Win32App::m_appPaused = false;
			Win32App::m_timer.Start();
		}
		return 0;
	case WM_CREATE:
		{	
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_GETMINMAXINFO:
		/* prevent too small window */
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
