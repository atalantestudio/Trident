// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/PhysicalDevice.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::PhysicalDevice::PhysicalDevice(const Trident::GraphicsAdapter* adapter, const DX12::Instance* instance, IDXGIAdapter* dxgiAdapter) :
		Trident::PhysicalDevice(adapter),
		adapter(dxgiAdapter),
		factory(instance->factory)
	{
		DXGI_ADAPTER_DESC adapterDescriptor;

		const HRESULT result = dxgiAdapter->GetDesc(&adapterDescriptor);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get adapter descriptor.");

		const LPCSTR description = ConvertUTF16ToUTF8(adapterDescriptor.Description);

		ADAPTER_INFO("Using [].", description);

		delete[] description;

		setLimits();
	}

	void DX12::PhysicalDevice::setLimits() {
		// TODO: Set other limits.

		limits.minUniformBufferOffsetAlignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
		limits.minStorageBufferOffsetAlignment = D3D12_RAW_UAV_SRV_BYTE_ALIGNMENT;
	}
}