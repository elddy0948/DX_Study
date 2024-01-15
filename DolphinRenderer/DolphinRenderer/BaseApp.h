#pragma once
#ifndef BASE_APP_H
#define BASE_APP_H

#include "Headers.h"

class BaseApp {
public:
	BaseApp(UINT width, UINT height, std::wstring name);
	virtual ~BaseApp();

public:
	virtual void OnInit() = 0;
	virtual void OnUpdate() = 0;
	virtual void OnRender() = 0;
	virtual void OnDestroy() = 0;

	UINT GetWidth() const { return m_width; }
	UINT GetHeight() const { return m_height; }
	const WCHAR* GetTitle() const { return m_title.c_str(); }

	void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);

protected:
	void GetHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter = false);

protected:
	UINT m_width;
	UINT m_height;
	bool m_useWarpDevice = false;

private:
	float m_aspectRatio;
	std::wstring m_title;
};

#endif /* !BASE_APP_H */
