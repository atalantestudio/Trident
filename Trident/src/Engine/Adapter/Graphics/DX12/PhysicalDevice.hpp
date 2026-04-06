// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace Trident {
	class DX12::PhysicalDevice final : public Trident::PhysicalDevice {
		public:
			explicit PhysicalDevice(const Trident::GraphicsAdapter* adapter, IDXGIAdapter* dxgiAdapter, const DXGI_ADAPTER_DESC& adapterDescriptor, IDXGIFactory4* factory);

		public:
			IDXGIAdapter* adapter;
			DXGI_ADAPTER_DESC adapterDescriptor;
			IDXGIFactory4* factory;
			sequence<char8> name;
	};

	namespace DX12 {
		ref<Trident::PhysicalDevice> selectPhysicalDevice(const Trident::Instance* instance, bool preferIntegratedGpu);

		view<char8> getPhysicalDeviceName(const Trident::PhysicalDevice* physicalDevice);
	}
}