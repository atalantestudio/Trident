// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/DX12/Device.hpp"
#include "Engine/Adapter/Graphics/DX12/Image.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace Trident {
	class DX12::Swapchain final : public Trident::Swapchain {
		public:
			explicit Swapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor);

		public:
			IDXGISwapChain3* swapChain;
			UINT syncInterval;
	};

	namespace DX12 {
		ref<Trident::Swapchain> createSwapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor);

		void destroySwapchain(const Trident::Device* device, const Trident::Swapchain* swapchain);

		void recreateSwapchain(const Trident::Device* device, Trident::Swapchain* swapchain, const SwapchainDescriptor& descriptor);

		void updateSwapchain(const Trident::Device* device, Trident::Swapchain* swapchain, const SwapchainDescriptor& descriptor);
	}
}