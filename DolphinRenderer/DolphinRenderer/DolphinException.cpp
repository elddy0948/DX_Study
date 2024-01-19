#include "DolphinException.h"

DolphinException::DolphinException(HRESULT hr, const std::wstring& functionName, const std::wstring& fileName, int lineNumber) :
	ErrorCode(hr),
	FunctionName(functionName),
	FileName(fileName),
	LineNumber(lineNumber)
{
}

std::wstring DolphinException::ToString() const
{
	_com_error err(ErrorCode);
	std::wstring msg = err.ErrorMessage();
	return FunctionName + L" failed in " + FileName + L"; line " + std::to_wstring(LineNumber) + L"; error : " + msg;
}