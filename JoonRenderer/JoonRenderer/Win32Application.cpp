#include "Win32Application.h"

HWND Win32Application::m_hwnd = nullptr;
GameTimer Win32Application::m_timer = GameTimer();

LRESULT CALLBACK Win32Application::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return BaseApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

int Win32Application::Run(BaseApp* pApp)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = pApp->AppInstance();
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"Register Class is Failed!", 0, 0);
		return false;
	}

	RECT rect = { 0, 0, pApp->GetWidth(), pApp->GetHeight() };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	m_hwnd = CreateWindow(
		L"MainWnd",
		pApp->GetTitle().c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		0,
		0,
		pApp->AppInstance(),
		0);

	if (!m_hwnd)
	{
		MessageBox(0, L"Create Window Failed!", 0, 0);
		return false;
	}

	pApp->Initialize();

	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);

	MSG msg = { 0 };
	m_timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_timer.Tick();
			if (!pApp->IsPaused())
			{
				CalculateFrameStats();
				pApp->Update();
				pApp->Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}

void Win32Application::CalculateFrameStats()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	if (m_timer.GameTime() - timeElapsed >= 1.0f)
	{
		float fps = (float)frameCount;
		float mspf = 1000.0f / fps;

		std::wstring fpsText = std::to_wstring(fps);
		std::wstring mspfText = std::to_wstring(mspf);

		std::wstring windowText = BaseApp::GetApp()->GetTitle() +
			L"    fps: " + fpsText +
			L"    mspf: " + mspfText;

		SetWindowText(m_hwnd, windowText.c_str());
		frameCount = 0;
		timeElapsed += 1.0f;
	}
}