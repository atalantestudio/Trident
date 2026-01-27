// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/PhysicalDevice.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	DX12::PhysicalDevice::PhysicalDevice(const DX12::Instance& instance) :
		adapter(instance.selectAdapter())
	{
		DXGI_ADAPTER_DESC adapterDescriptor;

		const HRESULT result = adapter->GetDesc(&adapterDescriptor);

		ADAPTER_ASSERT(SUCCEEDED(result), "Failed to get adapter descriptor.");

		const LPCSTR description = ConvertUTF16ToUTF8(adapterDescriptor.Description);

		ADAPTER_INFO("Using [].", description);

		delete[] description;
	}
}