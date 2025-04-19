#pragma once

#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class VulkanSwapchain : public Swapchain, public ObjectWrapper<VkSwapchainKHR> {
		public:
			VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, VkDevice device, const SwapchainDescriptor& descriptor);

			~VulkanSwapchain() override;

			void recreate(const SwapchainDescriptor& descriptor) override;

			uint32 getExtentWidth() const override;
			uint32 getExtentHeight() const override;

			void construct(const SwapchainDescriptor& descriptor);
			void destroy();

		public:
			chain<VkImage> images;
			chain<VkImageView> imageViews;
			chain<VkFramebuffer> framebuffers;

		private:
			const VulkanPhysicalDevice& physicalDevice;
			const VkDevice device;
			const VulkanSurface& surface;
	};
}