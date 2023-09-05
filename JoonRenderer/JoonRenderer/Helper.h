#pragma once

#include <string>
#include <Windows.h>
#include <d3d12.h>
#include <d3dcommon.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <fstream>

#include "DxException.h"

class Helper
{
public:
	static UINT CalculateConstantBufferByteSize(UINT byteSize);
	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);
	static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(
		const std::wstring& filename);
};

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif