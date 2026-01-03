#pragma once

#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace ProjectA {
	class VulkanDevice;

	class VulkanSwapchain final : public Swapchain, public ObjectWrapper<VkSwapchainKHR> {
		public:
			explicit VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const SwapchainDescriptor& descriptor);

			~VulkanSwapchain() final override;

			uint32 getImageCount() const final override;

			void update(const SwapchainDescriptor& descriptor) final override;

		private:
			void recreate(const SwapchainDescriptor& descriptor);

		private:
			const VulkanPhysicalDevice& physicalDevice;
			const VulkanDevice& device;
			const VulkanSurface& surface;
			uint32 imageCount = 0;
	};
}