#include "App.h"

#pragma comment(lib, "d2d1")

int APIENTRY wWinMain(
	HINSTANCE hInstance, 
	HINSTANCE, 
	LPWSTR lpCmdLine, 
	int nCmdShow)
{
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		MainApp app;
		if (SUCCEEDED(app.Initialize()))
		{
			app.RunMessageLoop();
		}
		CoUninitialize();
	}
	return 0;
}

MainApp::MainApp()
{
	engine = new Engine();
}

MainApp::~MainApp()
{}

HRESULT MainApp::Initialize()
{
	HRESULT hr = S_OK;

	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = MainApp::WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = HINST_THISCOMPONENT;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.hCursor = LoadCursor(NULL, IDI_APPLICATION);
	windowClass.lpszClassName = L"D2D Main App";

	ATOM x = RegisterClassEx(&windowClass);

	m_hWnd = CreateWindowEx(
		NULL,
		L"D2D Main App",
		L"Game",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		RESOLUTION_X,
		RESOLUTION_Y,
		NULL,
		NULL,
		HINST_THISCOMPONENT,
		this
	);

	hr = m_hWnd ? S_OK : E_FAIL;

	RECT rect1;
	GetWindowRect(m_hWnd, &rect1);
	RECT rect2;
	GetClientRect(m_hWnd, &rect2);

	SetWindowPos(
		m_hWnd,
		NULL,
		rect1.left,
		rect1.top,
		RESOLUTION_X + ((rect1.right - rect1.left) - (rect2.right - rect2.left)),
		RESOLUTION_Y + ((rect1.bottom - rect1.top) - (rect2.bottom - rect2.top)),
		NULL
	);

	if (SUCCEEDED(hr))
	{
		engine->InitializeD2D(m_hWnd);
		ShowWindow(m_hWnd, SW_SHOWNORMAL);
		UpdateWindow(m_hWnd);
	}

	return hr;
}

void MainApp::RunMessageLoop()
{
	MSG msg;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	int frames = 0;
	double framesTime = 0;

	boolean running = true;

	while (running)
	{
		end = std::chrono::steady_clock::now();

		double elapsed_secs = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() / 1000000.0;
		begin = end;

		// Display FPS
		framesTime += elapsed_secs;
		frames++;

		if (framesTime > 1)
		{
			WCHAR fpsText[32];
			swprintf(fpsText, 32, L"Game: %d FPS", frames);
			SetWindowText(m_hWnd, fpsText);
			frames = 0;
			framesTime = 0;
		}

		// Messages and user input
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				running = false;
			}
		}

		// Game Logic
		engine->Logic(elapsed_secs);
		// Drawing 
		engine->Draw();
	}
}

LRESULT CALLBACK MainApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (uMsg == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		MainApp* pMainApp = (MainApp*)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hWnd,
			GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(pMainApp)
		);

		result = 1;
	}
	else
	{
		MainApp* pMainApp = reinterpret_cast<MainApp*>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
				hWnd,
				GWLP_USERDATA
			)));
		bool wasHandled = false;
		if (pMainApp)
		{
			switch (uMsg)
			{
			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hWnd, NULL, FALSE);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_KEYDOWN:
			{
				pMainApp->engine->KeyDown(wParam);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_KEYUP:
			{
				pMainApp->engine->KeyUp(wParam);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_MOUSEMOVE:
			{
				pMainApp->engine->MousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_LBUTTONUP:
			{
				pMainApp->engine->MouseButtonUp(true, false);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_LBUTTONDOWN:
			{
				pMainApp->engine->MouseButtonDown(true, false);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_RBUTTONUP:
			{
				pMainApp->engine->MouseButtonUp(false, true);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_RBUTTONDOWN:
			{
				pMainApp->engine->MouseButtonDown(false, true);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			result = 1;
			wasHandled = true;
			break;
			}
		}
		if (!wasHandled)
		{
			result = DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	return result;
}