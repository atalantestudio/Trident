// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PhysicalDevice.hpp"

namespace Trident {
	class DX12::PhysicalDevice final : public Trident::PhysicalDevice {
		public:
			explicit PhysicalDevice(const Trident::GraphicsAdapter* adapter, IDXGIAdapter* dxgiAdapter, IDXGIFactory4* factory);

		public:
			IDXGIAdapter* adapter;

			// Used when creating a swapchain from this physical device.
			IDXGIFactory4* factory;
	};
}