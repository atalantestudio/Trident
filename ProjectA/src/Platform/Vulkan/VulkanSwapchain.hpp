#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/Swapchain.hpp"

#include "Platform/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	class VulkanSwapchain : public Swapchain, public ObjectWrapper<VkSwapchainKHR> {
		public:
			VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VkDevice device);

			~VulkanSwapchain();

		private:
			const VkDevice device;
			std::vector<VkImage> images;
			std::vector<VkImageView> imageViews;
	};
}