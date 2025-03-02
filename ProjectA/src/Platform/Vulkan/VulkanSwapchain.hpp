#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/Swapchain.hpp"

#include "Platform/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	class VulkanSwapchain : public Swapchain, public ObjectWrapper<VkSwapchainKHR> {
		public:
			VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VkDevice device, const SwapchainDescriptor& descriptor);

			~VulkanSwapchain();

			uint32 getExtentWidth() const override;
			uint32 getExtentHeight() const override;

		public:
			const VkExtent2D imageExtent;
			std::vector<VkImage> images;
			std::vector<VkImageView> imageViews;
			std::vector<VkFramebuffer> framebuffers;

		private:
			const VkDevice device;
	};
}