#include "DxException.h"

DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber)
	: ErrorCode(hr), mFunctionName(functionName), mFileName(fileName), mLineNumber(lineNumber)
{
}

std::wstring DxException::ToString() const
{
	_com_error err(ErrorCode);
	std::wstring msg = err.ErrorMessage();
	return mFunctionName + L" failed in " + mFileName + L"; line : " + std::to_wstring(mLineNumber) + L"; error : " + msg;
}