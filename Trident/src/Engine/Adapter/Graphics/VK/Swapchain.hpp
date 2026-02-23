// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace Trident {
	class VK::Swapchain final : public Trident::Swapchain {
		public:
			explicit Swapchain(const SwapchainDescriptor& descriptor, const VK::Surface& surface, const VK::Device& device);

			~Swapchain() final override;

			void update(const SwapchainDescriptor& descriptor) final override;

		private:
			void recreate(const SwapchainDescriptor& descriptor);

		public:
			VkSwapchainKHR swapchain;

		private:
			const VK::Surface& surface;
			const VK::Device& device;
	};
}