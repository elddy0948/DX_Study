#pragma once
#ifndef HEADERS_H
#define HEADERS_H

/* Windows */
#include <Windows.h>
#include <wrl.h>
#include <comdef.h>

/* DirectX */
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include "d3dx12.h"

/* Standard */
#include <iostream>
#include <cassert>
#include <mutex>
#include <memory>
#include <exception>

#include "DolphinException.h"

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize) {
    if (path == nullptr) { throw std::exception(); }
    DWORD size = GetModuleFileName(nullptr, path, pathSize);
    if (size == 0 || size == pathSize) { throw std::exception(); }

    WCHAR* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash) { *(lastSlash + 1) = L'\0'; }
}
#endif /* !HEADERS_H */
