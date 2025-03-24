#pragma once

#include <vulkan/vulkan.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Swapchain.hpp"

#include "Backend/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	class VulkanSwapchain : public Swapchain, public ObjectWrapper<VkSwapchainKHR> {
		public:
			VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, VulkanSurface& surface, VkDevice device, const SwapchainDescriptor& descriptor);

			~VulkanSwapchain() override;

			void recreate(const SwapchainDescriptor& descriptor) override;

			uint32 getExtentWidth() const override;
			uint32 getExtentHeight() const override;

			void construct(const SwapchainDescriptor& descriptor);
			void destruct();

		public:
			VkExtent2D imageExtent;
			std::vector<VkImage> images;
			std::vector<VkImageView> imageViews;
			std::vector<VkFramebuffer> framebuffers;

		private:
			const VulkanPhysicalDevice& physicalDevice;
			const VkDevice device;
			VulkanSurface& surface;
	};
}