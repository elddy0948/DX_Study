#include "Helper.h"

UINT Helper::CalculateConstantBufferByteSize(UINT byteSize)
{
	return (byteSize + 255) & ~255;
}