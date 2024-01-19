#pragma once
#ifndef DOLPHINE_EXCEPTION_H
#define DOLPHINE_EXCEPTION_H

#include "Headers.h"

class DolphinException {
public:
	DolphinException() = default;
	DolphinException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber);

public:
	std::wstring ToString() const;

public:
	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring FileName;
	int LineNumber = -1;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)    \
{   \
    HRESULT hr__ = (x);   \
    std::wstring wfn = AnsiToWString(__FILE__); \
    if(FAILED(hr__)) { throw DolphinException(hr__, L#x, wfn, __LINE__);}   \
}
#endif

#endif /* !DOLPHINE_EXCEPTION_H */
