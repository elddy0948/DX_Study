#include "Device.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

int main(void) {
	Device gpu;

	gpu.initialize();

	return 0;
}