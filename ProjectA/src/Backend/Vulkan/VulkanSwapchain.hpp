#pragma once

#include "Backend/Vulkan/VulkanSurface.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Swapchain.hpp"

namespace ProjectA {
	class VulkanDevice;

	class VulkanSwapchain : public Swapchain, public ObjectWrapper<VkSwapchainKHR> {
		public:
			VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const SwapchainDescriptor& descriptor);

			~VulkanSwapchain() final override;

			uint32 getImageCount() const final override;

			void update(const SwapchainDescriptor& descriptor) final override;

			void getImages(const ref<RenderPass>& renderPass) final override;

			void construct(const SwapchainDescriptor& descriptor);

			void destroy();

		public:
			std::vector<VkImage> images;
			std::vector<VkImageView> imageViews;
			std::vector<VkFramebuffer> framebuffers;

		private:
			const VulkanPhysicalDevice& physicalDevice;
			const VulkanDevice& device;
			const VulkanSurface& surface;
			uint32 imageCount = 0;
	};
}