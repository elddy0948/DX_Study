#pragma once

#include <Windows.h>
#include <string>
#include <comdef.h>
#include <fstream>

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber);
	
	std::wstring ToString() const;
	
	HRESULT ErrorCode = S_OK;
	std::wstring mFunctionName;
	std::wstring mFileName;
	int mLineNumber = -1;
};