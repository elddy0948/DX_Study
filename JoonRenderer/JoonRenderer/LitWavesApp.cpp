#include "LitWavesApp.h"

// FrameResource
FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT wavesVertexCount, UINT materialCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocator.GetAddressOf())));
	ObjectConstantsBuffer = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
	PassConstantsBuffer = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	VertexBuffer = std::make_unique<UploadBuffer<Vertex>>(device, wavesVertexCount, false);
	MaterialConstantsBuffer = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
}

FrameResource::~FrameResource() {}

// LitWavesApp
LitWavesApp::LitWavesApp(HINSTANCE hInstance) : BaseApp(hInstance) {}

LitWavesApp::~LitWavesApp()
{
	BaseApp::~BaseApp();
}

void LitWavesApp::UpdateMaterialConstantBuffers()
{
	auto currentMaterialCB = m_currentFrameResource->MaterialConstantsBuffer.get();

	for (auto& material : m_materials)
	{
		Material* mat = material.second.get();

		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX materialTransform = XMLoadFloat4x4(&mat->MaterialTransform);

			MaterialConstants materialConstants;
			materialConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			materialConstants.FresnelR0 = mat->FresnelR0;
			materialConstants.Roughness = mat->Roughness;

			currentMaterialCB->CopyData(mat->ConstantBufferIndex, materialConstants);

			mat->NumFramesDirty--;
		}
	}
}

void LitWavesApp::DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems)
{
	UINT objectCBByteSize = Helper::CalculateConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = Helper::CalculateConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = m_currentFrameResource->ObjectConstantsBuffer->Resource();
	auto matCB = m_currentFrameResource->MaterialConstantsBuffer->Resource();

	for (size_t i = 0; i < renderItems.size(); ++i)
	{
		auto renderItem = renderItems[i];

		commandList->IASetVertexBuffers(0, 1, &renderItem->Geo->VertexBufferView());
		commandList->IASetIndexBuffer(&renderItem->Geo->IndexBufferView());
		commandList->IASetPrimitiveTopology(renderItem->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objectCBAddress = objectCB->GetGPUVirtualAddress() + renderItem->ObjectConstantsBufferIndex * objectCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + renderItem->Material->ConstantBufferIndex * matCBByteSize;

		commandList->SetGraphicsRootConstantBufferView(0, objectCBAddress);
		commandList->SetGraphicsRootConstantBufferView(1, matCBAddress);

		commandList->DrawIndexedInstanced(renderItem->IndexCount, 1, renderItem->StartIndexLocation, renderItem->BaseVertexLocation, 0);
	}
}


void LitWavesApp::BuildMaterials()
{
	auto grass = std::make_unique<Material>();
	grass->Name = "grass";
	grass->ConstantBufferIndex = 0;
	grass->DiffuseAlbedo = XMFLOAT4(0.2f, 0.6f, 0.6f, 1.0f);
	grass->Roughness = 0.125f;

	auto water = std::make_unique<Material>();
	water->Name = "water";
	water->ConstantBufferIndex = 1;
	water->DiffuseAlbedo = XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
	water->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	water->Roughness = 0.0f;

	m_materials["grass"] = std::move(grass);
	m_materials["water"] = std::move(water);
}