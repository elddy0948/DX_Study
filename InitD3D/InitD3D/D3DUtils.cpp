#include "D3DUtils.h"
#include <comdef.h>

DxException::DxException(
	HRESULT hr,
	const std::wstring& functionName,
	const std::wstring& filename,
	int lineNumber)
	: ErrorCode(hr),
	FunctionName(functionName),
	Filename(filename),
	LineNumber(lineNumber)
{
}

std::wstring DxException::ToString() const
{
	_com_error err(ErrorCode);
	std::wstring msg = err.ErrorMessage();

	return FunctionName + L"Failed in " 
		+ Filename + L"line " + std::to_wstring(LineNumber) 
		+ L"; error : " + msg;
}