#pragma once

#include <Windows.h>
#include <string>
#include <cassert>
#include <d3d12.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <memory>

class D3DHelper
{

};

class com_exception : public std::exception
{
public:
	com_exception(HRESULT hr) : result(hr) {}

	const char* what() const noexcept override
	{
		static char s_str[64] = {};
		sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
		return s_str;
	}

private:
	HRESULT result;
};

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw com_exception(hr);
	}
}