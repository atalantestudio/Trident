// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Swapchain.hpp"

namespace Trident {
	class VK::Swapchain final : public Trident::Swapchain {
		public:
			explicit Swapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor);

		public:
			VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	};

	namespace VK {
		ref<Trident::Swapchain> createSwapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor);

		void destroySwapchain(const Trident::Device* device, const Trident::Swapchain* swapchain);

		void recreateSwapchain(const Trident::Device* device, Trident::Swapchain* swapchain, const SwapchainDescriptor& descriptor);

		void updateSwapchain(const Trident::Device* device, Trident::Swapchain* swapchain, const SwapchainDescriptor& descriptor);
	}
}