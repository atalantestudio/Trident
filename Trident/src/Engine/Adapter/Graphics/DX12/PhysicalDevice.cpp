// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/PhysicalDevice.hpp"

namespace Trident {
	DX12::PhysicalDevice::PhysicalDevice(const Trident::GraphicsAdapter* adapter, IDXGIAdapter* dxgiAdapter, IDXGIFactory4* factory) :
		Trident::PhysicalDevice(adapter),
		adapter(dxgiAdapter),
		factory(factory)
	{
		limits.minUniformBufferOffsetAlignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
		limits.minStorageBufferOffsetAlignment = D3D12_RAW_UAV_SRV_BYTE_ALIGNMENT;
	}
}