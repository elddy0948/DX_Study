#pragma once
#ifndef HEADERS_H
#define HEADERS_H

/* Windows */
#include <Windows.h>

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


/* https://github.com/Microsoft/DirectXTK/wiki/ThrowIfFailed#enhancements */
class com_exception : public std::exception
{
public:
    com_exception(HRESULT hr) : result(hr) {}

    const char* what() const noexcept override
    {
        static char s_str[64] = {};
        sprintf_s(s_str, "Failure with HRESULT of %08X",
            static_cast<unsigned int>(result));
        return s_str;
    }

private:
    HRESULT result;
};

// Helper utility converts D3D API failures into exceptions.
inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw com_exception(hr);
    }
}

inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize) {
    if (path == nullptr) { throw std::exception(); }
    DWORD size = GetModuleFileName(nullptr, path, pathSize);
    if (size == 0 || size == pathSize) { throw std::exception(); }

    WCHAR* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash) { *(lastSlash + 1) = L'\0'; }
}
#endif /* !HEADERS_H */
