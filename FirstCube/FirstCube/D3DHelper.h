#pragma once

#include <Windows.h>
#include <string>
#include <cassert>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <memory>
#include "d3dx12.h"
#include <exception>
#include <xstring>
#include "DxException.h"

class D3DHelper
{

};

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{ \
	HRESULT hr__ = (x); \
	std::wstring wfn = AnsiToWString(__FILE__); \
	if (FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif