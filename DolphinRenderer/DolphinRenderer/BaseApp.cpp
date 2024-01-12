#include "BaseApp.h"

BaseApp::BaseApp() {
	m_hwnd = 0;
	m_hInstance = GetModuleHandle(nullptr);
}

BaseApp::~BaseApp() {

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_LBUTTONDOWN:
		MessageBox(0, L"Hello, World", L"Hello", MB_OK);
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void BaseApp::Initialize() {
	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"BaseWndClass";

	if (!RegisterClass(&wc)) {
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
		return;
	}

	m_hwnd = CreateWindow(
		L"BaseWndClass",
		L"Win32Basic",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		m_hInstance,
		0);

	if (m_hwnd == 0) {
		MessageBox(0, L"Create Window FAILED", 0, 0);
		return;
	}

	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);

	std::cout << m_hInstance;
}

int BaseApp::Run() {
	MSG msg = { 0 };
	BOOL bRet = 1;

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// Game Loop...
		}
	}
	return (int)msg.wParam;
}